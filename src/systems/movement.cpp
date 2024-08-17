//
// Created by henry on 8/17/24.
//
#include "movement.h"

#include <hagame/core/scene.h>

#include "../components/body.h"
#include "../common/gameState.h"
#include "imgui.h"

using namespace hg;

void Movement::onUpdate(double dt) {

    ImGui::Begin("Movement");

    auto state = GameState::Get();
    auto input = state->input;

    scene->entities.forEach<Body>([&](Body* body, Entity* entity) {
        entity->transform.position += body->velocity * dt;
        body->velocity += body->acceleration * dt;
        float speed = body->velocity.magnitude();
        if (speed > 0) {
            body->velocity -= body->velocity.normalized() * speed * body->friction * dt;
        }
    });

    scene->entities.forEach<Player>([&](Player* player, Entity* entity) {
        auto body = entity->getComponent<Body>();
        Vec3 dir = Vec3(input.axes[Axes::MoveX], input.axes[Axes::MoveY], 0);
        dir.normalize();
        body->acceleration = dir * player->acceleration;

        ImGui::SeparatorText("Player");
        ImGui::SliderFloat("Accel", &player->acceleration, 0, 100);
        ImGui::SliderFloat("Friction", &body->friction, 0, 100);
    });

    ImGui::End();

}
