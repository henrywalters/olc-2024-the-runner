//
// Created by henry on 8/16/24.
//
#include "runtime.h"

#include <hagame/core/assets.h>
#include <hagame/ui/elements/image.h>
#include <hagame/ui/elements/label.h>
#include <hagame/ui/elements/divider.h>
#include <hagame/utils/profiler.h>

#include "../systems/renderer.h"
#include "../systems/movement.h"
#include "../systems/ysorter.h"

#include "../components/resource.h"
#include "../components/spriteSheet.h"
#include "../components/ysort.h"

#include "../components/inventory.h"
#include "../components/startPoint.h"
#include "../components/uiFrame.h"

#include "../game.h"

using namespace hg;
using namespace hg::graphics;

Runtime::Runtime(hg::graphics::Window *window):
    m_window(window)
{

}

void Runtime::onInit() {
    addSystem<Renderer>(m_window);
    addSystem<YSorter>();
    addSystem<Movement>();

    auto state = GameState::Get();

    m_player = entities.add();
    m_player->name = "Player";
    m_player->addComponent<Player>();
    m_player->addComponent<Body>();
    m_player->addComponent<Inventory>();
    m_player->addComponent<YSort>()->sortPoint = hg::Vec2(0, -0.3);
    auto collider = m_player->addComponent<math::components::CircleCollider>();
    collider->radius = 0.25;
    collider->pos = Vec2(0, -0.25);
    auto ss = m_player->addComponent<SpriteSheetComponent>();
    ss->spriteGroup = "walk_towards";
    ss->texture = "player";
    ss->spriteSheet = "player";
    ss->size = hg::Vec2(1.0);

    auto arrow = m_player->add();
    auto arrowSprite = arrow->addComponent<Sprite>();
    arrowSprite->texture = "arrow";
    arrowSprite->size = hg::Vec2::Identity();
    arrowSprite->color = Color::white();
    arrow->transform.position = hg::Vec3(4, 0, 500);

    m_home = entities.add();
    m_home->name = "House";
    m_home->addComponent<YSort>()->sortPoint = hg::Vec2(0, -2);
    m_home->addComponent<Body>()->dynamic = false;
    auto houseSprite = m_home->addComponent<Sprite>();
    houseSprite->texture = "home";
    houseSprite->size = hg::Vec2(4, 4);
    houseSprite->color = Color::white();

    auto houseColl = m_home->addComponent<math::components::RectCollider>();
    houseColl->pos = hg::Vec2(-1.5, -1.9);
    houseColl->size = hg::Vec2(3, 2);

    auto startPos = m_home->addComponent<StartPoint>();
    startPos->position = hg::Vec3(0, -2.0, 0);
    startPos->radius = 1;

    auto map = getTexture("map")->image.get();
    auto labels = getTexture("labels")->image.get();

    m_mapBuilder = std::make_unique<MapBuilder>(this, map, labels);
    m_mapBuilder->build(m_home->transform.position);

    addHUD();
    addDeathScreen();
    addSurvivedScreen();
    addMenuScreen();

    startNewDay(false, true);
}



void Runtime::startNewDay(bool addResources, bool clearInventory) {

    auto state = GameState::Get();

    state->elapsedTime = 0;

    state->paused = false;
    m_survivedScreen->getComponent<UIFrame>()->active = false;
    m_deathScreen->getComponent<UIFrame>()->active = false;

    auto movement = getSystem<Movement>();

    entities.forEach<Body>([&](Body* body, Entity* entity) {
        body->velocity = Vec3::Zero();
    });

    entities.forEach<Resource>([&](auto* resource, auto* entity) {
        entities.remove(entity);
    });

    entities.forEach<Inventory>([&](Inventory* inventory, auto* entity) {
        inventory->clear();
    });

    if (clearInventory) {
        state->totalInventory.clear();
        state->daysPassed = 0;
    }

    m_player->transform.position = m_home->position() + m_home->getComponent<StartPoint>()->position;
    m_mapBuilder->addResources();

    movement->bakeStaticColliders();
}

void Runtime::onUpdate(double dt) {

    utils::Profiler::Start("Runtime::onUpdate");

    auto state = GameState::Get();

    if (state->paused) {
        return;
    }

    if (state->input.buttonsPressed[Buttons::Pause]) {
        state->paused = !state->paused;
        m_menuScreen->getComponent<UIFrame>()->active = state->paused;
    }

    state->elapsedTime += dt;

    state->atHome = false;

    m_timeDisplay->setText(state->getTime().toString());
    m_timeDisplay->getBuffer()->hAlignment(TextHAlignment::Left);

    Vec2 delta;

    entities.forEach<StartPoint>([&](StartPoint* start, Entity* startEntity) {
        auto startCenter = startEntity->transform.position + start->position;
        entities.forEach<Player>([&](Player* player, Entity* playerEntity) {
            delta = (playerEntity->position() - startCenter).resize<2>();

            if (delta.magnitudeSq() <= state->homeRadius * state->homeRadius) {
                state->atHome = true;
            }
        });
        // Debug::DrawCircle(startCenter[0], startCenter[1], state->homeRadius, Color::blue(), 2. / PIXELS_PER_METER);
    });

    auto arrow = m_player->getComponentInChildren<Sprite>();

    if (state->atHome) {
        arrow->entity->active = false;
    } else {
        arrow->entity->active = true;
        auto rot = Quat(std::atan2(-delta.y(), -delta.x()), hg::Vec3(0, 0, 1));
        arrow->entity->transform.position = rot.rotatePoint(hg::Vec3(1, 0, 0));
        arrow->entity->transform.rotation = rot;
        arrow->entity->transform.position *= 4;
        arrow->entity->transform.position[2] = 500;
    }

    if (state->elapsedTime >= state->levelTime) {

        state->paused = true;

        if (state->atHome) {

            state->daysPassed++;

            entities.forEach<Inventory>([&](Inventory* inventory, auto* entity) {
                for (const auto &e: *ENUMS(ResourceType)) {
                    state->totalInventory.add(e.key, inventory->count(e.key));
                }
            });

            std::string message = "Collected: \n";

            for (const auto& e : *ENUMS(ResourceType)) {
                message += e.label + ": " + std::to_string(GameState::Get()->totalInventory.count(e.key)) + "\n";
            }

            m_totalDisplay->setText(message);
            m_survivedDisplay->setText("You Survived Day " + std::to_string(state->daysPassed) + "!");

            m_survivedScreen->getComponent<UIFrame>()->active = true;
        } else {
            m_deathScreen->getComponent<UIFrame>()->active = true;
        }

        state->elapsedTime = 0;
    }

    utils::Profiler::End("Runtime::onUpdate");
}

void Runtime::addSurvivedScreen() {

    const float ITEM_SIZE = 100;
    const float UI_WIDTH = GAME_SIZE[0] * 0.5;
    const float UI_HEIGHT = GAME_SIZE[1] * 0.75;
    const float PADDING = 15;

    m_survivedScreen = entities.add(m_player);
    m_survivedScreen->name = "SurvivedScreen";
    auto ui = m_survivedScreen->addComponent<UIFrame>(m_window->size());
    ui->active = false;

    auto root = ui->frame.addElement<ui::Container>();
    root->style.margin = ui::offset((GAME_SIZE[0] - UI_WIDTH) / 2, (GAME_SIZE[1] - UI_HEIGHT) / 2, (GAME_SIZE[0] - UI_WIDTH) / 2, (GAME_SIZE[1] - UI_HEIGHT) / 2);
    root->style.backgroundColor = MENU_BG;
    root->style.padding = ui::offset(PADDING);

    struct MenuItem {
        std::string label;
        std::function<void()> onSelect;
    };

    std::vector<MenuItem> menuItems {
            {
                    "Go to Next Day",
                    [&]() {
                        startNewDay(false, false);
                    }
            },
            {
                    "Quit to Main Menu",
                    [&]() {
                        m_survivedScreen->getComponent<UIFrame>()->active = false;
                        startNewDay(false, true);
                        game()->scenes()->activate("main_menu");
                    }
            },
    };

#ifndef __EMSCRIPTEN__
menuItems.push_back({
        "Quit to Desktop",
        [&]() {
            game()->running(false);
        }
});
#endif
    std::vector rowSizes {0.15, 0.25, 0.1};

    for (int i = 0; i < menuItems.size(); i++) {
        rowSizes.push_back(0.5 / menuItems.size());
    }

    auto menu = ui->frame.addElementTo<ui::GridContainer>(root, ui::GridConfig::Custom(hg::Vec2i(1, menuItems.size() + 3), {1.0}, rowSizes));

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 0));
        m_survivedDisplay = ui->frame.addElementTo<ui::Label>(gridEl, Vec2::Zero(), getFont("8bit_lg"), "You Survived!");
    }

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 1));
        m_totalDisplay = ui->frame.addElementTo<ui::Label>(gridEl, Vec2::Zero(), getFont("8bit"), "");
    }

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 2));
        auto divider = ui->frame.addElementTo<ui::Divider>(gridEl, 5);
        divider->style.foregroundColor = SECONDARY;
    }

    for (int i = 0; i < menuItems.size(); i++) {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), hg::Vec2i(0, i + 3));
        gridEl->style.margin = ui::offset(5);
        addButton(&ui->frame, gridEl, menuItems[i].label, menuItems[i].onSelect);
    }

}

void Runtime::addMenuScreen() {

    const float ITEM_SIZE = 100;
    const float UI_WIDTH = GAME_SIZE[0] * 0.5;
    const float UI_HEIGHT = GAME_SIZE[1] * 0.75;
    const float PADDING = 15;

    m_menuScreen = entities.add(m_player);
    m_menuScreen->name = "Menu";
    auto ui = m_menuScreen->addComponent<UIFrame>(m_window->size());
    ui->active = false;

    auto root = ui->frame.addElement<ui::Container>();
    root->style.margin = ui::offset((GAME_SIZE[0] - UI_WIDTH) / 2, (GAME_SIZE[1] - UI_HEIGHT) / 2, (GAME_SIZE[0] - UI_WIDTH) / 2, (GAME_SIZE[1] - UI_HEIGHT) / 2);
    root->style.backgroundColor = MENU_BG;
    root->style.padding = ui::offset(PADDING);

    struct MenuItem {
        std::string label;
        std::function<void()> onSelect;
    };

    std::vector<MenuItem> menuItems {
            {
                    "Resume",
                    [&]() {
                        m_menuScreen->getComponent<UIFrame>()->active = false;
                        auto state = GameState::Get();
                        state->paused = false;
                    }
            },
            {
                    "Restart Day",
                    [&]() {
                        m_menuScreen->getComponent<UIFrame>()->active = false;
                        startNewDay(false, false);
                    }
            },
            {
                    "Quit to Main Menu",
                    [&]() {
                        m_menuScreen->getComponent<UIFrame>()->active = false;
                        startNewDay(false, true);
                        game()->scenes()->activate("main_menu");
                    }
            },
    };

#ifndef __EMSCRIPTEN__
menuItems.push_back({
        "Quit to Desktop",
        [&]() {
            game()->running(false);
        }
});
#endif
    std::vector rowSizes {0.15, 0.1};

    for (int i = 0; i < menuItems.size(); i++) {
        rowSizes.push_back(0.75 / menuItems.size());
    }

    auto menu = ui->frame.addElementTo<ui::GridContainer>(root, ui::GridConfig::Custom(hg::Vec2i(1, menuItems.size() + 2), {1.0}, rowSizes));

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 0));
        ui->frame.addElementTo<ui::Label>(gridEl, Vec2::Zero(), getFont("8bit_lg"), "Paused");
    }

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 1));
        auto divider = ui->frame.addElementTo<ui::Divider>(gridEl, 5);
        divider->style.foregroundColor = SECONDARY;
    }

    for (int i = 0; i < menuItems.size(); i++) {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), hg::Vec2i(0, i + 2));
        gridEl->style.margin = ui::offset(5);
        addButton(&ui->frame, gridEl, menuItems[i].label, menuItems[i].onSelect);
    }
}

void Runtime::addDeathScreen() {
    auto state = GameState::Get();

    const float ITEM_SIZE = 100;
    const float UI_WIDTH = GAME_SIZE[0] * 0.5;
    const float UI_HEIGHT = GAME_SIZE[1] * 0.5;
    const float PADDING = 15;

    m_deathScreen = entities.add(m_player);
    m_deathScreen->name = "DeathScreen";
    auto ui = m_deathScreen->addComponent<UIFrame>(m_window->size());
    ui->active = false;

    auto root = ui->frame.addElement<ui::Container>();
    root->style.margin = ui::offset((GAME_SIZE[0] - UI_WIDTH) / 2, (GAME_SIZE[1] - UI_HEIGHT) / 2, (GAME_SIZE[0] - UI_WIDTH) / 2, (GAME_SIZE[1] - UI_HEIGHT) / 2);
    root->style.backgroundColor = MENU_BG;
    root->style.padding = ui::offset(PADDING);

    struct MenuItem {
        std::string label;
        std::function<void()> onSelect;
    };

    std::vector<MenuItem> menuItems {
        {
            "Restart Day",
            [&]() {
                startNewDay(false, false);
            }
        },
        {
            "Quit to Main Menu",
            [&]() {
                m_deathScreen->getComponent<UIFrame>()->active = false;
                startNewDay(false, true);
                game()->scenes()->activate("main_menu");
            }
        },
    };

#ifndef __EMSCRIPTEN__
menuItems.push_back({
    "Quit to Desktop",
    [&]() {
        game()->running(false);
    }
});
#endif
    std::vector rowSizes {0.15, 0.15, 0.1};

    for (int i = 0; i < menuItems.size(); i++) {
        rowSizes.push_back(0.6 / menuItems.size());
    }

    auto menu = ui->frame.addElementTo<ui::GridContainer>(root, ui::GridConfig::Custom(hg::Vec2i(1, menuItems.size() + 3), {1.0}, rowSizes));

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 0));
        ui->frame.addElementTo<ui::Label>(gridEl, Vec2::Zero(), getFont("8bit_lg"), "You Died");
    }

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 1));
        ui->frame.addElementTo<ui::Label>(gridEl, Vec2::Zero(), getFont("8bit"), "You must return home before the day ends!");
    }


    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), Vec2i(0, 2));
        auto divider = ui->frame.addElementTo<ui::Divider>(gridEl, 5);
        divider->style.foregroundColor = SECONDARY;
    }

    for (int i = 0; i < menuItems.size(); i++) {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(menu, menu->config(), hg::Vec2i(0, i + 3));
        gridEl->style.margin = ui::offset(5);
        addButton(&ui->frame, gridEl, menuItems[i].label, menuItems[i].onSelect);
    }

}

void Runtime::addHUD() {

    auto state = GameState::Get();

    const float ITEM_SIZE = 100;
    const float UI_WIDTH = 200;
    const float UI_HEIGHT = 200;
    const float PADDING = 5;

    auto entity = entities.add(m_player);
    entity->name = "HUD";
    auto ui = entity->addComponent<UIFrame>(m_window->size());

    // inventoryContainer->style.backgroundColor = graphics::Color(255, 255, 255, 100);
    auto resourceTypes = *ENUMS(ResourceType);
    auto config = ui::GridConfig::Uniform(hg::Vec2i(1, resourceTypes.size() + 1));
    auto grid = ui->frame.addElement<ui::GridContainer>(config);
    grid->style.margin = ui::offset(PADDING, GAME_SIZE[1] - UI_HEIGHT - PADDING, GAME_SIZE[0] - UI_WIDTH - PADDING, PADDING);

    auto resourceSS = getSpriteSheet("resources");

    {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(grid, config, hg::Vec2i(0, 0));
        m_timeDisplay = ui->frame.addElementTo<ui::Label>(gridEl, Vec2::Zero(), getFont("8bit"), "00:00");
    }

    for (const auto& e : resourceTypes) {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(grid, config, hg::Vec2i(0, 1 + e.key));
        auto itemContainer = ui->frame.addElementTo<ui::Container>(gridEl);
        // itemContainer->style.backgroundColor = state->random.color();
        itemContainer->style.margin = ui::offset(PADDING);

        auto itemGridConfig = ui::GridConfig::Custom(hg::Vec2i(2, 1), {0.25, 0.75}, {1.0});
        auto itemGrid = ui->frame.addElementTo<ui::GridContainer>(itemContainer, itemGridConfig);

        auto imageGridEl = ui->frame.addElementTo<ui::GridElement>(itemGrid, itemGridConfig, hg::Vec2i(0, 0));
        auto labelGridEl = ui->frame.addElementTo<ui::GridElement>(itemGrid, itemGridConfig, hg::Vec2i(1, 0));

        auto texture = getTexture("resources");
        auto rect = resourceSS->atlas.getRect(hg::Vec2i(e.key, 0), texture->image->size);
        auto image = ui->frame.addElementTo<ui::Image>(imageGridEl, texture, rect);

        auto label = ui->frame.addElementTo<ui::Label>(labelGridEl, hg::Vec2::Zero(), getFont("8bit"), "0 / " + std::to_string(state->mapResourceCounts[e.key]));
    }
}




