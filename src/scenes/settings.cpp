//
// Created by henry on 7/21/24.
//
#include "settings.h"
#include "../common/gameState.h"
#include "../systems/renderer.h"
#include "../common/actionMap.h"
#include "../common/ui.h"

#include <hagame/core/game.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/debug.h>
#include <hagame/graphics/shaders/color.h>
#include <hagame/graphics/shaders/texture.h>
#include <hagame/graphics/windows.h>
#include <hagame/ui/elements/gridContainer.h>
#include <hagame/ui/elements/textButton.h>
#include <hagame/ui/elements/divider.h>
#include <hagame/graphics/shaders/text.h>
#include <hagame/ui/elements/label.h>
#include <hagame/ui/elements/checkbox.h>

using namespace hg;
using namespace hg::graphics;

Settings::Settings(hg::graphics::Window *window):
        m_window(window),
        m_menu(Rect(Vec2::Zero(), window->size().cast<float>())),
        m_quad(window->size().cast<float>(), Vec2(0, 0), true),
        m_mesh(&m_quad) {

    m_devMode = GameState::Get()->persistentSettings.devMode.value();
    m_vsync = GameState::Get()->persistentSettings.vsync.value();

    m_menu.root()->style.margin = hg::ui::offset(1. / 3, 1. / 6, 1. / 3, 1. / 6, ui::Unit::Percent);
    m_menu.root()->style.padding = hg::ui::offset(25);
    m_menu.root()->style.backgroundColor = MENU_BG;

    auto config = ui::GridConfig::Custom(hg::Vec2i(1, 5), {1.0}, {0.15, 0.1, 0.25, 0.25, 0.25});

    auto grid = m_menu.addElement<ui::GridContainer>(config);

    {
        auto gridEl = m_menu.addElementTo<ui::GridElement>(grid, config, Vec2i(0, 0));
        m_menu.addElementTo<ui::Label>(gridEl, Vec2::Zero(), getFont("8bit_lg"), "Settings");
    }

    {
        auto gridEl = m_menu.addElementTo<ui::GridElement>(grid, config, Vec2i(0, 1));
        auto divider = m_menu.addElementTo<ui::Divider>(gridEl, 5);
        divider->style.foregroundColor = SECONDARY;
    }

    {
        auto gridEl = m_menu.addElementTo<ui::GridElement>(grid, config, Vec2i(0, 2));
        m_vsyncCbox = addCheckbox(&m_menu, gridEl, "VSync", &m_vsync);
    }

    {
        auto gridEl = m_menu.addElementTo<ui::GridElement>(grid, config, Vec2i(0, 3));
        m_devModeCbox = addCheckbox(&m_menu, gridEl, "Developer Mode", &m_devMode);
    }

    {
        auto gridEl = m_menu.addElementTo<ui::GridElement>(grid, config, Vec2i(0, 4));
        addButton(&m_menu, gridEl, "Main Menu", [&](){
            game()->scenes()->activate("main_menu");
        });
    }
}

void Settings::onInit() {

    m_renderPasses.create(RenderMode::Color, m_window->size());

    Windows::Events.subscribe(WindowEvents::Resize, [&](Window* window) {
        resize();
    });
}

void Settings::onUpdate(double dt) {

    auto state = GameState::Get();

    state->input = m_window->input.player(ACTION_MAP, 0);

    if (state->input.buttonsPressed[Buttons::Select]) {
        m_menu.root()->trigger(ui::UITriggers::Select);
    }

    auto rawMousePos = m_window->input.devices.keyboardMouse()->mousePosition();
    rawMousePos[1] = m_window->size()[1] - rawMousePos[1];

    m_menu.update(rawMousePos, dt);

    state->persistentSettings.devMode = m_devModeCbox->checked();
    state->persistentSettings.vsync = m_vsyncCbox->checked();

    m_batchRenderer.quads.clear();
    m_batchRenderer.sprites.clear();

    m_window->color(Color::black());
    m_window->clear();

    auto shader = getShader(COLOR_SHADER.name);

    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    Debug::Render();

    m_renderPasses.render(RenderMode::Color, 1);

    m_menu.render(dt, false);

    shader = getShader(BATCH_COLOR_SHADER.name);
    shader->use();
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("view", Mat4::Identity());

    m_batchRenderer.quads.render();

    shader = getShader(BATCH_TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("view", Mat4::Identity());

    m_batchRenderer.sprites.render();
}

void Settings::resize() {
    auto size = m_window->size();

    m_quad.size(size.cast<float>());
    m_mesh.update(&m_quad);

    m_renderPasses.resize(RenderMode::Color, size);
}

void Settings::onAfterUpdate() {
    auto state = GameState::Get();
    m_devModeCbox->checked(state->persistentSettings.devMode.value());
    m_vsyncCbox->checked(state->persistentSettings.vsync.value());
}
