//
// Created by henry on 8/16/24.
//
#include "runtime.h"

#include <hagame/core/assets.h>

#include "../systems/renderer.h"
#include "../systems/movement.h"
#include "../systems/ysorter.h"

#include "../components/body.h"
#include "../components/player.h"
#include "../components/resource.h"
#include "../components/tile.h"
#include "../components/spriteSheet.h"
#include "../components/ysort.h"

#include "../common/tiles.h"
#include "../components/tree.h"
#include "../common/mapBuilder.h"

using namespace hg;
using namespace hg::graphics;

Runtime::Runtime(hg::graphics::Window *window):
    m_window(window)
{

}

void Runtime::onInit() {
    addSystem<Renderer>(m_window);
    auto movement = addSystem<Movement>();
    addSystem<YSorter>();

    auto state = GameState::Get();

    auto player = entities.add();
    player->name = "Player";
    player->addComponent<Player>();
    player->addComponent<Body>();
    player->addComponent<YSort>()->sortPoint = hg::Vec2(0, -0.3);
    auto collider = player->addComponent<math::components::CircleCollider>();
    collider->radius = 0.25;
    collider->pos = Vec2(0, -0.25);
    auto ss = player->addComponent<SpriteSheetComponent>();
    ss->spriteGroup = "walk_towards";
    ss->texture = "player";
    ss->spriteSheet = "player";
    ss->size = hg::Vec2(1.0);

    auto map = getTexture("map")->image.get();
    auto labels = getTexture("labels")->image.get();

    MapBuilder mapBuilder(this, map, labels);
    mapBuilder.build(player->transform.position);

    movement->bakeStaticColliders();

    for (int i = 0; i < 10; i++) {
        auto entity = entities.add();
        auto resource = entity->addComponent<Resource>();
        entity->transform.position = state->random.realVector<3, float>(-5, 5);
    }
}

void Runtime::onUpdate(double dt) {

}

