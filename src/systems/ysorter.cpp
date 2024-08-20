//
// Created by henry on 8/18/24.
//

#include "ysorter.h"
#include <hagame/core/scene.h>

#include "renderer.h"

#include "../components/ysort.h"
#include "imgui.h"

void YSorter::onUpdate(double dt) {
    std::vector<YSort*> sortables;

    auto renderer = scene->getSystem<Renderer>();

    scene->entities.forEach<YSort>([&](YSort* sort, hg::Entity* entity) {
        if (renderer->inView(entity->position())) {
            sortables.push_back(sort);
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

}