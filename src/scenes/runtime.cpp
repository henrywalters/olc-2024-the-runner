//
// Created by henry on 8/16/24.
//
#include "runtime.h"

#include <hagame/core/assets.h>
#include <hagame/ui/elements/image.h>
#include <hagame/ui/elements/label.h>

#include "../systems/renderer.h"
#include "../systems/movement.h"
#include "../systems/ysorter.h"

#include "../components/resource.h"
#include "../components/spriteSheet.h"
#include "../components/ysort.h"

#include "../components/inventory.h"
#include "../components/startPoint.h"
#include "../components/uiFrame.h"

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

    const float ITEM_SIZE = 100;
    const float UI_WIDTH = 200;
    const float UI_HEIGHT = 200;
    const float PADDING = 5;

    auto ui = entities.add(m_player)->addComponent<UIFrame>(m_window->size());
    // inventoryContainer->style.backgroundColor = graphics::Color(255, 255, 255, 100);
    auto resourceTypes = *ENUMS(ResourceType);
    auto config = ui::GridConfig::Uniform(hg::Vec2i(1, resourceTypes.size()));
    auto grid = ui->frame.addElement<ui::GridContainer>(config);
    float width = resourceTypes.size() * ITEM_SIZE + (resourceTypes.size() + 1) * PADDING;
    grid->style.margin = ui::offset(PADDING, GAME_SIZE[1] - UI_HEIGHT - PADDING, GAME_SIZE[0] - UI_WIDTH - PADDING, PADDING);

    auto resourceSS = getSpriteSheet("resources");

    for (const auto& e : resourceTypes) {
        auto gridEl = ui->frame.addElementTo<ui::GridElement>(grid, config, hg::Vec2i(0, e.key));
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

    m_home = entities.add();
    m_home->name = "House";
    m_home->addComponent<YSort>()->sortPoint = hg::Vec2(0, -2);
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

    startNewDay();
}


void Runtime::startNewDay() {

    auto movement = getSystem<Movement>();

    entities.forEach<Resource>([&](auto* resource, auto* entity) {
        entities.remove(entity);
    });

    entities.forEach<Inventory>([&](Inventory* inventory, auto* entity) {
        inventory->clear();
    });

    m_player->transform.position = m_home->position() + m_home->getComponent<StartPoint>()->position;
    m_mapBuilder->addResources();

    movement->bakeStaticColliders();
}

void Runtime::onUpdate(double dt) {

    auto state = GameState::Get();

    if (state->paused) {
        return;
    }

    state->elapsedTime += dt;

    if (state->elapsedTime >= state->levelTime) {

        startNewDay();

        state->daysPassed++;
        state->elapsedTime = 0;
    }

}


