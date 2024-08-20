//
// Created by henry on 8/19/24.
//

#include <hagame/core/scene.h>
#include <hagame/core/entity.h>
#include <hagame/core/assets.h>

#include "mapBuilder.h"
#include "../components/tile.h"
#include "../components/tree.h"
#include "../components/ysort.h"
#include "../components/resource.h"
#include "../components/body.h"

using namespace hg;

MapBuilder::MapBuilder(hg::Scene* scene, hg::graphics::Image* map, hg::graphics::Image* labels):
    m_scene(scene),
    m_map(map),
    m_labels(labels)
{}

void MapBuilder::build(hg::Vec3& playerPos) {

    auto state = GameState::Get();

    state->mapTiles.clear();
    state->mapProps.clear();

    for (int h = 0; h < m_map->size[0]; h++) {
        for (int v = 0; v < m_map->size[1]; v++) {
            auto pixel = m_map->getColor(hg::Vec2i(h, v));
            auto label = m_labels->getColor(hg::Vec2i(h, v));
            int tileIndex;
            if (getTileByColor(pixel, tileIndex)) {
                auto entity = m_scene->entities.add();
                entity->addComponent<MapTile>()->tileIndex = tileIndex;
                entity->transform.position = hg::Vec3(h, m_map->size[1] - v, 0) * MAP_TILE_METERS;

                addTile(entity, hg::Vec2i(h, v), TILES[tileIndex].type);

                state->mapTiles.insert(entity->transform.position.resize<2>(), MAP_TILE_METERS, entity);
            }

            if (m_labels->getColor(hg::Vec2i(h, v)) == hg::graphics::Color::red()) {
                playerPos = hg::Vec3(h * MAP_TILE_METERS, (m_map->size[1] - v) * MAP_TILE_METERS, 0);
            }
        }
    }
}

void MapBuilder::addTile(hg::Entity* entity, hg::Vec2i pos, TileType::type type) {
    if (type == TileType::Water && hasNonNeighbor(pos, type)) {
        auto collider = entity->addComponent<math::components::RectCollider>();
        collider->size = Vec2(MAP_TILE_METERS);
        collider->pos = collider->size * -0.5;
        return;
    }

    if (type == TileType::Grass && !hasNonNeighbor(pos, type)) {
        if (spawnTree(pos)) {
            return;
        }

        if (spawnResource(pos, type)) {
            return;
        }
    }
}

bool MapBuilder::hasNonNeighbor(hg::Vec2i pos, TileType::type type) {
    bool hasNeighbor = false;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (pos[0] + i >= 0 && pos[0] + i < m_map->size[0] && pos[1] + j >= 0 && pos[1] + j < m_map->size[1]) {
                auto neighbor = m_map->getColor(hg::Vec2i(pos[0] + i, pos[1] + j));
                int neighborpos;
                if (getTileByColor(neighbor, neighborpos)) {
                    if (TILES[neighborpos].type != type) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool MapBuilder::spawnTree(hg::Vec2i pos) {
    auto state = GameState::Get();
    auto rand = state->random.real<float>(0, 1);
    if (rand >= 1.0 - TREE_DENSITY) {
        auto tree = m_scene->entities.add();
        tree->transform.scale = Vec3(3.);
        auto ss = hg::getSpriteSheet("trees");
        auto index = Vec2i(state->random.integer(0, ss->atlas.size[0]), state->random.integer(0, ss->atlas.size[1]));
        tree->addComponent<TreeComponent>()->index = index;
        auto coll = tree->addComponent<math::components::CircleCollider>();
        coll->radius = 0.45;
        coll->pos = hg::Vec2(0, -1.5);
        tree->addComponent<YSort>()->sortPoint = hg::Vec2(0, -1.5);
        tree->transform.position = hg::Vec3(pos[0] * MAP_TILE_METERS, (m_map->size[1] - pos[1]) * MAP_TILE_METERS, 0);

        // Enable for funsies
        tree->addComponent<Body>();

        state->mapProps.insert(tree->transform.position.resize<2>(), MAP_TILE_METERS, tree);

        return true;
    }

    return false;
}

bool MapBuilder::spawnResource(hg::Vec2i pos, ResourceType::type type) {
    auto state = GameState::Get();
    auto rand = state->random.real<float>(0, 1);

    if (rand >= 1.0 - RESOURCES[type].rarity) {

        auto entity = m_scene->entities.add();
        entity->addComponent<Resource>()->resourceIndex = type;
        auto coll = entity->addComponent<math::components::CircleCollider>();
        coll->radius = MAP_TILE_METERS * 0.5;
        entity->addComponent<YSort>();
        entity->addComponent<Body>();
        entity->transform.position = hg::Vec3(pos[0] * MAP_TILE_METERS, (m_map->size[1] - pos[1]) * MAP_TILE_METERS, 0);

        state->mapProps.insert(entity->transform.position.resize<2>(), MAP_TILE_METERS, entity);

        return true;
    }

    return false;
}


