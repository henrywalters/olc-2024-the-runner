//
// Created by henry on 2/29/24.
//
#include <hagame/graphics/shaders/text.h>
#include <hagame/graphics/components/spriteSheetAnimator.h>
#include <hagame/utils/profiler.h>
#include <hagame/graphics/shaders/texture.h>
#include <hagame/graphics/components/textRenderer.h>
#include <hagame/graphics/shaders/color.h>
#include "renderer.h"
#include "../constants.h"
#include "../components/player.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;
using namespace hg::math::collisions;
using namespace hg::math::components;

HG_SYSTEM(Graphics, Renderer)

Renderer::Renderer(hg::graphics::Window *window):
    m_window(window),
    m_displayQuad(window->size().cast<float>(), Vec2(0, 0), true),
    m_display(&m_displayQuad),
    m_animQuad(Vec2(1, 1)),
    m_anim(&m_animQuad)
{
    m_displayQuad.centered(false);
    m_display.update(&m_displayQuad);
}

void Renderer::onInit() {
    camera.zoom = 1.;
    camera.size = GAME_SIZE.div(PIXELS_PER_METER).cast<float>();
    camera.centered = true;

    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);
    m_renderPasses.create(RenderMode::Color, GAME_SIZE);
    m_renderPasses.create(RenderMode::Lighting, GAME_SIZE);
    m_renderPasses.create(RenderMode::Debug, GAME_SIZE);
    m_renderPasses.create(RenderMode::UI, GAME_SIZE);
    m_renderPasses.create(RenderMode::Combined, GAME_SIZE);

    auto colorShader = getShader("color");
    auto textShader = getShader(TEXT_SHADER.name);
    auto font = getFont("8bit");

    Debug::Initialize(colorShader, textShader, font);
}

void Renderer::onBeforeUpdate() {
    m_batchRenderer.quads.clear();
    m_batchRenderer.sprites.clear();
}

void Renderer::onRender(double dt) {

    auto state = GameState::Get();

    Profiler::Start("Renderer::onRender");

    m_window->color(Color::black());

    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);
    m_renderPasses.clear(RenderMode::Color, Color::black());
    m_renderPasses.clear(RenderMode::Lighting, Color::black());
    m_renderPasses.clear(RenderMode::Debug, Color::black());
    m_renderPasses.clear(RenderMode::UI, Color::black());
    m_renderPasses.clear(RenderMode::Combined, Color::black());

    m_renderPasses.bind(RenderMode::Color);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    Debug::ENABLED = state->debugLevel >= DebugLevel::Light;

    auto shader = getShader(TEXT_SHADER.name);
    shader->use();

    shader->setMat4("projection", Mat4::Orthographic(0, GAME_SIZE[0], 0, GAME_SIZE[1], -100, 100));
    shader->setMat4("view", Mat4::Identity());

    shader = getShader("color");
    shader->use();
    shader->setMat4("projection", camera.projection());
    shader->setMat4("view", camera.view());

    m_window->setVSync(state->persistentSettings.vsync);

    Profiler::Start("Renderer::colorPass");
    colorPass(dt);
    Profiler::End("Renderer::colorPass");
    Profiler::Start("Renderer::lightPass");
    lightPass(dt);
    Profiler::End("Renderer::lightPass");
    Profiler::Start("Renderer::debugPass");
    debugPass(dt);
    Profiler::End("Renderer::debugPass");
    Profiler::End("Renderer::onRender");

    uiPass(dt);
    combinedPass(dt);

    camera.update(dt);
}

void Renderer::onUpdate(double dt) {
    scene->entities.forEach<components::SpriteSheetAnimator>([&](auto animator, auto entity) {
        animator->update(dt);
    });
}

void Renderer::colorPass(double dt) {
    auto shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("projection", camera.projection());
    shader->setMat4("view", camera.view());

    scene->entities.forEach<Quad>([&](auto quad, auto entity) {
        m_batchRenderer.quads.batch(entity, quad);
    });

    scene->entities.forEach<Player>([&](auto player, Entity* entity) {
        Debug::DrawRect(Rect(entity->transform.position.resize<2>(), Vec2::Identity()), Color::blue(), 2.0 / PIXELS_PER_METER);
    });

    scene->entities.forEach<Sprite>([&](auto sprite, auto entity) {
        //m_batchRenderer.sprites.batch(entity, sprite);
        m_batchRenderer.sprites.batch(
                sprite->texture,
                sprite->size,
                sprite->offset,
                Rect(Vec2(0.5, 0.5), Vec2(0.25, 0.25)),
                sprite->color,
                entity->model()
            );
    });

    shader = getShader(BATCH_COLOR_SHADER.name);
    shader->use();
    shader->setMat4("projection", camera.projection());
    shader->setMat4("view", camera.view());

    m_batchRenderer.quads.render();

    shader = getShader(BATCH_TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("projection", camera.projection());
    shader->setMat4("view", camera.view());

    m_batchRenderer.sprites.render();

    // Render any animated sprites above other stuff for automatic Z sorting
    shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("projection", camera.projection());
    shader->setMat4("view", camera.view());

    scene->entities.forEach<components::SpriteSheetAnimator>([&](auto animator, auto entity) {
        auto animation = (SpriteSheet*) animator->player->get();
        if (animation) {
            animation->texture()->bind();
            shader->setMat4("model", entity->model());
            auto rect = animation->getRect();
            m_animQuad.size(animator->size);
            m_animQuad.offset(animator->offset);
            m_animQuad.texSize(rect.size);
            m_animQuad.texOffset(rect.pos);
            m_anim.update(&m_animQuad);
            m_anim.render();
        }
    });

    shader = getShader(TEXT_BUFFER_SHADER.name);
    shader->use();
    shader->setMat4("view", camera.view());
    shader->setMat4("projection", camera.projection());

    scene->entities.forEach<TextRenderer>([&](auto text, auto entity) {
        shader->setMat4("model", entity->model());
        shader->setVec4("textColor", text->color);
        text->buffer()->render();
    });
    shader->setVec4("textColor", Color::white());

    m_renderPasses.render(RenderMode::Color, 1);
}

void Renderer::lightPass(double dt) {
    m_renderPasses.bind(RenderMode::Lighting);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    m_renderPasses.render(RenderMode::Lighting, 1);
}

void Renderer::debugPass(double dt) {
    m_renderPasses.bind(RenderMode::Debug);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    if (GameState::Get()->debugLevel == DebugLevel::Heavy) {
        scene->entities.forEach<RectCollider>([&](RectCollider *coll, Entity *entity) {
            Debug::DrawRect(Rect(coll->pos + entity->position().resize<2>(), coll->size), Color::blue(),
                            1.0 / PIXELS_PER_METER);
        });

        scene->entities.forEach<CircleCollider>([&](CircleCollider *coll, Entity *entity) {
            Vec2 pos = entity->transform.position.resize<2>();
            Debug::DrawCircle(pos[0] + coll->pos[0], pos[1] + coll->pos[1], coll->radius, Color::blue(),
                              1.0 / PIXELS_PER_METER);
        });
    }

    auto shader = getShader("color");
    shader->use();
    shader->setMat4("projection", camera.projection());
    shader->setMat4("view", camera.view());
    Debug::Render();

    m_renderPasses.render(RenderMode::Debug, 1);
}

void Renderer::uiPass(double dt) {
    m_renderPasses.bind(RenderMode::UI);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    m_renderPasses.render(RenderMode::UI, 1);
}

void Renderer::combinedPass(double dt) {

    auto state = GameState::Get();

    glDisable(GL_DEPTH_TEST);
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // m_renderPasses.bind(RenderMode::Combined);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    auto shader = getShader("combined");
    shader->use();

    shader->setInt("colorTex", 0);
    shader->setInt("lightTex", 1);
    shader->setInt("debugTex", 2);
    shader->setInt("uiTex", 3);
    shader->setFloat("useLighting", 0);
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, GAME_SIZE[0], 0, GAME_SIZE[1], -100, 100));
    shader->setMat4("model", Mat4::Identity());

    glActiveTexture(GL_TEXTURE0 + 0);
    m_renderPasses.get(RenderMode::Color)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 1);
    m_renderPasses.get(RenderMode::Lighting)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 2);
    m_renderPasses.get(RenderMode::Debug)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 3);
    m_renderPasses.get(RenderMode::UI)->texture->bind();

    m_display.render();

    glActiveTexture(GL_TEXTURE0 + 0);

    m_renderPasses.unbind(RenderMode::Combined);
    glViewport(0, 0, m_window->size()[0], m_window->size()[1]);
}

void Renderer::renderTile(hg::Vec2i index, hg::Vec3 position) {
    auto sheet = getSpriteSheet("tilesheet");
    auto texture = getTexture("tilesheet");
    Rect rect = sheet->atlas.getRect(index, texture->image->size);
    Mat4 model = Mat4::Translation(position);
    m_batchRenderer.sprites.batch("tilesheet", Vec2(1.), Vec2::Identity(), rect, Color::white(), model);
}