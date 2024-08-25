//
// Created by henry on 8/18/24.
//

#include "ysorter.h"
#include <hagame/core/scene.h>
#include <hagame/utils/profiler.h>

#include "renderer.h"

#include "../components/ysort.h"
#include "imgui.h"
#include "../components/body.h"

using namespace hg::utils;

void YSorter::onUpdate(double dt) {

    auto state = GameState::Get();

    Profiler::Start("YSorter");

    std::vector<YSort*> sortables;

    auto renderer = scene->getSystem<Renderer>();

    for (auto& entity : state->mapProps.getNeighbors(renderer->camera.transform.position.resize<2>(), maxBlocks().cast<float>())) {
        if (entity->hasComponent<YSort>()) {
            sortables.push_back(entity->getComponent<YSort>());
        }
    }

    for (auto& entity : state->mapResources.getNeighbors(renderer->camera.transform.position.resize<2>(), maxBlocks().cast<float>())) {
        if (entity->hasComponent<YSort>()) {
            sortables.push_back(entity->getComponent<YSort>());
        }
    }

    scene->entities.forEach<Body>([&](Body* body, hg::Entity* entity) {
        if (entity->hasComponent<YSort>()) {
            sortables.push_back(entity->getComponent<YSort>());
        }
    });

    std::sort(sortables.begin(), sortables.end(), [&](YSort* a, YSort* b) {
        auto posA = a->entity->position()[1] + a->sortPoint[1];
        auto posB = b->entity->position()[1] + b->sortPoint[1];
        auto idA = a->id();
        auto idB = b->id();
        if (posA == posB) {
            return idA > idB;
        } else {
            return posA > posB;
        }
    });

    for (int i = 0; i < sortables.size(); i++) {
        sortables[i]->entity->transform.position[2] = i + 1;
    }

    Profiler::End("YSorter");
}