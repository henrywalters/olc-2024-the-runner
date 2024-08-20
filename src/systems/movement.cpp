//
// Created by henry on 8/17/24.
//
#include "movement.h"

#include <hagame/core/scene.h>

#include "renderer.h"

#include "../components/body.h"
#include "../common/gameState.h"
#include "imgui.h"
#include "../components/spriteSheet.h"

using namespace hg;

void Movement::onUpdate(double dt) {

    ImGui::Begin("Movement");

    auto state = GameState::Get();
    auto input = state->input;
    auto map = getTexture("map")->image.get();

    scene->entities.forEach<math::components::RectCollider>([&](auto coll, Entity* entity) {
        m_colliderMap.insert(entity->position().resize<2>(), coll->size, entity);
    });

    scene->entities.forEach<math::components::CircleCollider>([&](auto coll, Entity* entity) {
        m_colliderMap.insert(entity->position().resize<2>(), coll->radius * 2, entity);
    });

    scene->entities.forEach<Body>([&](Body* body, Entity* entity) {

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

            if (neighbor == entity) {
                continue;
            }

            float t;

            auto neighborColl = neighbor->getComponent<math::components::CircleCollider>();
            auto coll = entity->getComponent<math::components::CircleCollider>();

            hg::Vec3 center = entity->position() + coll->pos.resize<3>();
            hg::Vec3 neighborCenter = neighbor->position() + neighborColl->pos.resize<3>();

            //graphics::Debug::DrawCircle(center.x(), center.y(), 1.0 / PIXELS_PER_METER, graphics::Color::blue(), 1. / PIXELS_PER_METER);
            //graphics::Debug::DrawCircle(neighborCenter.x(), neighborCenter.y(), 1.0 / PIXELS_PER_METER, graphics::Color::red(), 1. / PIXELS_PER_METER);

            auto hit = math::collisions::checkEntityAgainstEntity(entity, neighbor);
            if (hit.has_value()) {

                auto neighborBody = neighbor->getComponent<Body>();
                hg::Vec3 neighborVel;
                auto delta = (neighborCenter - center).resize<2>().resize<3>();
                auto normal = delta.normalized();
                auto distance = delta.magnitude();
                if (neighborBody) {
                    neighborVel = neighborBody->velocity;
                }
                auto relativeVel = neighborVel - body->velocity;
                float velAlongNormal = relativeVel.dot(normal);

                if (velAlongNormal > 0) {
                    continue;
                }

                float depth = neighborColl->radius + coll->radius - distance;

                if (neighborBody) {
                    auto correction = normal * depth / 2.0f;
                    entity->transform.position -= correction;
                    neighbor->transform.position += correction;
                    body->velocity += normal * 2.0 * velAlongNormal;
                    neighborBody->velocity -= normal * 2.0 * velAlongNormal;
                } else {
                    auto correction = normal * depth;
                    entity->transform.position -= correction;
                    body->velocity += normal * velAlongNormal;
                }
            }
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
    });

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

}
