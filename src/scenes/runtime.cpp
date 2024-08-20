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

using namespace hg;
using namespace hg::graphics;

Runtime::Runtime(hg::graphics::Window *window):
    m_window(window)
{

}

void Runtime::onInit() {
    addSystem<Renderer>(m_window);
    addSystem<Movement>();
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

    for (int h = 0; h < map->size[0]; h++) {
        for (int v = 0; v < map->size[1]; v++) {
            auto pixel = map->getColor(hg::Vec2i(h, v));
            auto label = labels->getColor(hg::Vec2i(h, v));
            int tileIndex;
            if (getTileByColor(pixel, tileIndex)) {
                auto entity = entities.add();
                entity->addComponent<MapTile>()->tileIndex = tileIndex;
                entity->transform.position = hg::Vec3(h, map->size[1] - v, 0) * MAP_TILE_METERS;

                if (TILES[tileIndex].type == TileType::Water) {
                    bool hasNeighbor = false;
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            if (i >= 0 && i < map->size[0] && j >= 0 && j < map->size[1]) {
                                auto neighbor = map->getColor(hg::Vec2i(h + i, v + j));
                                int neighborIndex;
                                if (getTileByColor(neighbor, neighborIndex)) {
                                    if (TILES[neighborIndex].type != TileType::Water) {
                                        hasNeighbor = true;
                                    }
                                }
                            }
                        }
                    }

                    if (hasNeighbor) {
                        auto collider = entity->addComponent<math::components::RectCollider>();
                        collider->size = Vec2(MAP_TILE_METERS);
                        collider->pos = collider->size * -0.5;
                    }
                } else if (TILES[tileIndex].type == TileType::Grass) {
                    auto rand = state->random.real<float>(0, 1);
                    if (rand >= 1.0 - TREE_DENSITY) {
                        auto tree = entities.add();
                        tree->transform.scale = Vec3(3.);
                        auto ss = hg::getSpriteSheet("trees");
                        auto index = Vec2i(state->random.integer(0, ss->atlas.size[0]), state->random.integer(0, ss->atlas.size[1]));
                        tree->addComponent<TreeComponent>()->index = index;
                        auto coll = tree->addComponent<math::components::CircleCollider>();
                        coll->radius = 0.35;
                        coll->pos = hg::Vec2(0, -1.5);
                        tree->addComponent<YSort>()->sortPoint = hg::Vec2(0, -1.5);
                        tree->transform.position = hg::Vec3(h * MAP_TILE_METERS, (map->size[1] - v) * MAP_TILE_METERS, 0);
                    }
                }

                state->mapTiles.insert(entity->transform.position.resize<2>(), MAP_TILE_METERS, entity);
            }

            if (label == Color::red()) {
                player->transform.position = hg::Vec3(h, map->size[1] - v, 1) * MAP_TILE_METERS;
            }
        }
    }

    for (int i = 0; i < 10; i++) {
        auto entity = entities.add();
        auto resource = entity->addComponent<Resource>();
        entity->transform.position = state->random.realVector<3, float>(-5, 5);
    }
}

void Runtime::onUpdate(double dt) {

}

