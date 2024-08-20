//
// Created by henry on 8/17/24.
//
#include "movement.h"

#include <hagame/core/scene.h>
#include <hagame/utils/profiler.h>

#include "renderer.h"

#include "../components/body.h"
#include "../common/gameState.h"
#include "imgui.h"
#include "../components/spriteSheet.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;

void Movement::onUpdate(double dt) {

    Profiler::Start("Movement");

    ImGui::Begin("Movement");

    auto state = GameState::Get();
    auto input = state->input;
    auto map = getTexture("map")->image.get();

    Profiler::Start("Prepare Grid");

    m_colliderMap.clear();

    auto renderer = scene->getSystem<Renderer>();

    std::vector<Body*> toCheck;

    scene->entities.forEach<Body>([&](Body* body, Entity* entity) {

        // Only care about moving objects twice the screen size
        if (!renderer->inView(entity->position(), 1)) {
            return;
        }
        if (entity->hasComponent<math::components::CircleCollider>()) {
            m_colliderMap.insert(entity->position().resize<2>(), entity->getComponent<math::components::CircleCollider>()->radius * 2, entity);
        } else {
            m_colliderMap.insert(entity->position().resize<2>(), entity->getComponent<math::components::RectCollider>()->size, entity);
        }

        toCheck.push_back(body);
    });

    Profiler::End("Prepare Grid");

    bool debug = false;

    for (auto body : toCheck) {

        auto entity = body->entity;

        float speed = body->velocity.magnitude();

        entity->transform.position += body->velocity * dt;

        body->velocity += body->acceleration * dt;
        speed = body->velocity.magnitude();

        if (speed < 0.1 && body->acceleration.magnitude() == 0) {
            body->velocity = Vec3::Zero();
        } else {
            body->velocity -= body->velocity.normalized() * speed * body->friction * dt;
        }

        for (const auto& neighbor : m_colliderMap.getNeighbors(entity->position().xy(), hg::Vec2(2 + speed * dt))) {
            handleCollision(entity, body, neighbor);
        }

        for (const auto& neighbor : m_staticColliderMap.getNeighbors(entity->position().xy(), hg::Vec2(2 + speed * dt))) {
            handleCollision(entity, body, neighbor);
        }

        if (entity->hasComponent<SpriteSheetComponent>()) {
            auto ss = entity->getComponent<SpriteSheetComponent>();
            if (speed > 1.0) {
                int fps = ((int) speed / 2) * 6;
                getSpriteSheet(ss->spriteSheet)->sprites.getSprite(ss->spriteGroup)->animated = true;
                getSpriteSheet(ss->spriteSheet)->sprites.getSprite(ss->spriteGroup)->fps = fps;
                ss->spriteGroup = body->velocity.y() > 0 ? "walk_away" : "walk_towards";
            } else {
                getSpriteSheet(ss->spriteSheet)->sprites.getSprite(ss->spriteGroup)->animated = false;
                getSpriteSheet(ss->spriteSheet)->sprites.getSprite(ss->spriteGroup)->reset();
            }
        }
    }

    scene->entities.forEach<Player>([&](Player* player, Entity* entity) {
        auto body = entity->getComponent<Body>();
        Vec3 dir = Vec3(input.axes[Axes::MoveX], input.axes[Axes::MoveY], 0);
        dir.normalize();
        body->acceleration = dir * player->acceleration;

        scene->getSystem<Renderer>()->camera.setPosition(entity->position().prod(PIXELS_PER_METER).floor().div(PIXELS_PER_METER));

        ImGui::SeparatorText("Player");
        ImGui::SliderFloat("Accel", &player->acceleration, 0, 500);
        ImGui::SliderFloat("Friction", &body->friction, 0, 500);
        ImGui::Text("Velocity: %s", body->velocity.toString().c_str());
        ImGui::Text("Pos: %s", entity->position().toString().c_str());
    });

    ImGui::End();

    Profiler::End("Movement");
}

void Movement::bakeStaticColliders() {

    m_staticColliderMap.clear();

    scene->entities.forEach<math::components::RectCollider>([&](auto coll, Entity* entity) {
        m_staticColliderMap.insert(entity->position().resize<2>(), coll->size, entity);
    });

    scene->entities.forEach<math::components::CircleCollider>([&](auto coll, Entity* entity) {
        m_staticColliderMap.insert(entity->position().resize<2>(), coll->radius * 2, entity);
    });
}

void Movement::handleCollision(hg::Entity *entity, Body *body, hg::Entity *neighbor) {
    if (neighbor == entity) {
        return;
    }

    auto hit = math::collisions::checkEntityAgainstEntity(entity, neighbor);
    if (hit.has_value()) {

        auto neighborBody = neighbor->getComponent<Body>();
        hg::Vec3 neighborVel;

        auto normal = hit->normal;
        auto depth = hit->depth;

        Debug::DrawLine(hit->position.resize<2>(), (hit->position + hit->normal).resize<2>(), Color::blue(), 2. / PIXELS_PER_METER);

        if (neighborBody) {
            neighborVel = neighborBody->velocity;
        }
        auto relativeVel = neighborVel - body->velocity;
        float velAlongNormal = relativeVel.dot(normal);

        if (velAlongNormal > 0) {
            return;
        }

        if (neighborBody) {
            auto correction = normal * depth / 2.0f;
            entity->transform.position -= correction;
            body->velocity += normal * velAlongNormal;

            neighbor->transform.position += correction;
            neighborBody->velocity -= normal * velAlongNormal;
        } else {
            auto correction = normal * depth;
            entity->transform.position -= correction;
            body->velocity += normal * velAlongNormal;
        }
    }
}
