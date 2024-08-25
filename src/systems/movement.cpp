//
// Created by henry on 8/17/24.
//
#include "movement.h"

#include <hagame/core/scene.h>
#include <hagame/utils/profiler.h>
#include <hagame/ui/elements/label.h>

#include "renderer.h"

#include "../components/body.h"
#include "../common/gameState.h"
#include "imgui.h"
#include "../components/spriteSheet.h"
#include "../components/resource.h"
#include "../components/inventory.h"
#include "../components/uiFrame.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;

void Movement::onUpdate(double dt) {

    auto state = GameState::Get();

    if (state->paused) {
        return;
    }

    Profiler::Start("Movement");

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
            auto coll = entity->getComponent<math::components::CircleCollider>();
            m_colliderMap.insert(entity->position().resize<2>() + coll->pos, coll->radius * 2, entity);
        } else {
            auto coll = entity->getComponent<math::components::RectCollider>();
            m_colliderMap.insert(entity->position().resize<2>() + coll->pos, coll->size, entity);
        }

        toCheck.push_back(body);
    });

    Profiler::End("Prepare Grid");

    bool debug = false;

    for (auto body : toCheck) {

        if (!body->dynamic) {
            continue;
        }

        auto entity = body->entity;

        float speed = body->velocity.magnitude();

        auto center = math::collisions::getCenter(entity);

        math::Ray movementRay(entity->position() + center, body->velocity * dt);

        Debug::DrawLine(movementRay.origin.resize<2>(), movementRay.getPointOnLine(1.0).resize<2>(), Color::green(), 2. / PIXELS_PER_METER);

        entity->transform.position += body->velocity * dt;

        body->velocity += body->acceleration * dt;
        speed = body->velocity.magnitude();

        if (speed < 0.1 && body->acceleration.magnitude() == 0) {
            body->velocity = Vec3::Zero();
        } else {
            body->velocity -= body->velocity.normalized() * speed * body->friction * dt;
        }

        for (const auto& neighbor : m_colliderMap.getNeighbors(entity->position().xy(), hg::Vec2(5))) {
            handleCollision(movementRay, entity, body, neighbor);
        }

        for (const auto& neighbor : m_staticColliderMap.getNeighbors(entity->position().xy(), hg::Vec2(5))) {
            handleCollision(movementRay, entity, body, neighbor);
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

    if (state->persistentSettings.devMode) {
        ImGui::Begin("Movement");
    }

    scene->entities.forEach<Player>([&](Player* player, Entity* entity) {
        auto inventory = entity->getComponent<Inventory>();
        auto body = entity->getComponent<Body>();
        Vec3 dir = Vec3(input.axes[Axes::MoveX], input.axes[Axes::MoveY], 0);
        dir.normalize();
        body->acceleration = dir * player->acceleration;

        scene->getSystem<Renderer>()->camera.setPosition(entity->position().prod(PIXELS_PER_METER).floor().div(PIXELS_PER_METER));
        if (state->persistentSettings.devMode) {
            ImGui::SeparatorText("Player");
            ImGui::SliderFloat("Accel", &player->acceleration, 0, 2000);
            ImGui::SliderFloat("Friction", &body->friction, 0, 500);
            ImGui::Text("Velocity: %s", body->velocity.toString().c_str());
            ImGui::Text("Pos: %s", entity->position().toString().c_str());

            ImGui::SeparatorText("Resources");
        }
        int total = 0;

        scene->entities.forEach<UIFrame>([&](UIFrame* ui, auto entity) {
            if (entity->name != "HUD") {
                return;
            }
            auto resourceUI = ui->frame.root()->children()[0];
            for (const auto& e : *ENUMS(ResourceType)) {
                total += inventory->count(e.key);
                ui::Label* label;
                hg::structures::Tree::DepthFirstTraverse(resourceUI->children()[e.key + 1], [&](structures::Tree* el) {
                    if (dynamic_cast<ui::Label*>(static_cast<ui::Element*>(el)) != nullptr) {
                        label = dynamic_cast<ui::Label*>(static_cast<ui::Element*>(el));
                        return false;
                    }
                    return true;
                });
                if (label) {
                    label->setText(std::to_string(inventory->count(e.key)) + " / " + std::to_string(
                            state->mapResourceCounts.find(e.key) == state->mapResourceCounts.end() ? 0
                                                                                                   : state->mapResourceCounts[e.key]));
                }
            }
        });


    });
    if (state->persistentSettings.devMode) {
        ImGui::End();
    }

    Profiler::End("Movement");
}

void Movement::bakeStaticColliders() {

    m_staticColliderMap.clear();

    scene->entities.forEach<math::components::RectCollider>([&](auto coll, Entity* entity) {
        if (entity->hasComponent<Body>()) { return ; }
        m_staticColliderMap.insert(entity->position().resize<2>(), coll->size, entity);
    });

    scene->entities.forEach<math::components::CircleCollider>([&](auto coll, Entity* entity) {
        if (entity->hasComponent<Body>()) { return ; }
        m_staticColliderMap.insert(entity->position().resize<2>(), coll->radius * 2, entity);
    });
}

void Movement::handleCollision(math::Ray movementRay, hg::Entity *entity, Body *body, hg::Entity *neighbor) {

    auto state = GameState::Get();

    if (neighbor == entity || !neighbor || !entity) {
        return;
    }

    auto tmpEntityZ = entity->transform.position[2];
    auto tmpNeighborZ = neighbor->transform.position[2];

    entity->transform.position[2] = 0;
    neighbor->transform.position[2] = 0;

    float t;

    auto rayHit = math::collisions::checkRayAgainstEntity(movementRay, neighbor, t);

    if (rayHit.has_value() && t >= 0 && t < 1) {
        entity->transform.position = movementRay.origin;
        if (neighbor->hasComponent<Resource>() && entity->hasComponent<Player>()) {
            entity->getComponent<Inventory>()->add(neighbor->getComponent<Resource>()->resourceIndex, 1);
            state->mapResources.remove(neighbor->position().resize<2>(), hg::Vec2(PIXELS_PER_METER), neighbor);
            m_staticColliderMap.remove(neighbor->position().resize<2>(), hg::Vec2(PIXELS_PER_METER), neighbor);
            scene->entities.remove(neighbor);
            return;
        }
    }

    auto hit = math::collisions::checkEntityAgainstEntity(entity, neighbor);

    entity->transform.position[2] = tmpEntityZ;
    neighbor->transform.position[2] = tmpNeighborZ;

    if (hit.has_value()) {

        if (neighbor->hasComponent<Resource>() && entity->hasComponent<Player>()) {
            entity->getComponent<Inventory>()->add(neighbor->getComponent<Resource>()->resourceIndex, 1);
            state->mapResources.remove(neighbor->position().resize<2>(), hg::Vec2(PIXELS_PER_METER), neighbor);
            m_staticColliderMap.remove(neighbor->position().resize<2>(), hg::Vec2(PIXELS_PER_METER), neighbor);
            scene->entities.remove(neighbor);
            return;
        }

        auto neighborBody = neighbor->getComponent<Body>();
        if (neighborBody && !neighborBody->dynamic) {
            neighborBody = nullptr;
        }
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
