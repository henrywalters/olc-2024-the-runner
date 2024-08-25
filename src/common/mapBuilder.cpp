//
// Created by henry on 8/19/24.
//

#include <hagame/core/scene.h>
#include <hagame/core/entity.h>
#include <hagame/core/assets.h>

#include "mapBuilder.h"
#include "../components/tile.h"
#include "../components/ysort.h"
#include "../components/resource.h"
#include "../components/body.h"
#include "../components/prop.h"
#include "props.h"

using namespace hg;

MapBuilder::MapBuilder(hg::Scene* scene, hg::graphics::Image* map, hg::graphics::Image* labels):
    m_scene(scene),
    m_map(map),
    m_labels(labels)
{}

void MapBuilder::addResources() {
    auto state = GameState::Get();

    state->mapResources.clear();
    state->mapResourceCounts.clear();

    for (int h = 0; h < m_map->size[0]; h++) {
        for (int v = 0; v < m_map->size[1]; v++) {
            auto pixel = m_map->getColor(hg::Vec2i(h, v));
            auto label = m_labels->getColor(hg::Vec2i(h, v));
            int tileIndex;
            if (getTileByColor(pixel, tileIndex)) {
                auto tile = TILES[tileIndex];
                if (tile.type == TileType::Grass && !hasNonNeighbor(hg::Vec2i(h, v), tile.type)) {

                    if (state->random.real<float>(0, 1) < (float) v / m_map->size[1]) {
                        spawnResource(hg::Vec2i(h, v), ResourceType::Wheat);
                    } else {
                        spawnResource(hg::Vec2i(h, v), ResourceType::Wood);
                    }


                }

                if (tile.type == TileType::Clay && !hasNonNeighbor(hg::Vec2i(h, v), tile.type)) {
                    spawnResource(hg::Vec2i(h, v), ResourceType::Stone);
                }
            }
        }
    }
}

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
        if (spawnProp(pos, PropType::Tree)) {
            return;
        }
    }

    if (type == TileType::Sand && !hasNonNeighbor(pos, type)) {
        if (spawnProp(pos, PropType::PalmTree)) {
            return;
        }
    }

    if (type == TileType::Clay && !hasNonNeighbor(pos, type)) {
        if (spawnProp(pos, PropType::Rock)) {
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

bool MapBuilder::spawnProp(hg::Vec2i pos, PropType::type type) {
    auto state = GameState::Get();
    auto ss = hg::getSpriteSheet("props");
    auto def = PROPS[type];

    auto rand = state->random.real<float>(0, 1);
    if (rand >= 1.0 - def.rarity) {

        auto scale = state->random.real<float>(def.minScale, def.maxScale);

        auto entity = m_scene->entities.add();
        entity->transform.scale = Vec3(scale);
        auto sprites = ss->sprites.getSprite(def.group)->group();
        auto prop = entity->addComponent<Prop>();
        prop->propIndex = type;
        prop->spriteIndex = state->random.integer<int>(0, sprites.size() - 1);

        auto coll = entity->addComponent<math::components::CircleCollider>();
        coll->radius = def.colliderRadius * scale;
        coll->pos = def.origin * scale;
        entity->addComponent<YSort>()->sortPoint = def.origin * scale;
        entity->transform.position = hg::Vec3(pos[0] * MAP_TILE_METERS, (m_map->size[1] - pos[1]) * MAP_TILE_METERS, 0) - def.origin.resize<3>() * scale;

        // Enable for funsies
        // tree->addComponent<Body>();

        state->mapProps.insert(entity->transform.position.resize<2>(), MAP_TILE_METERS, entity);

        return true;
    }

    return false;
}

bool MapBuilder::spawnResource(hg::Vec2i pos, ResourceType::type type) {
    auto state = GameState::Get();
    auto rand = state->random.real<float>(0, 1);

    if (state->mapProps.getNeighbors(hg::Vec2(pos[0] * MAP_TILE_METERS, (m_map->size[1] - pos[1]) * MAP_TILE_METERS), hg::Vec2(1.0)).size() > 0) {
        return false;
    }

    if (rand >= 1.0 - RESOURCES[type].rarity) {

        auto entity = m_scene->entities.add();
        entity->addComponent<Resource>()->resourceIndex = type;
        auto coll = entity->addComponent<math::components::CircleCollider>();
        coll->radius = MAP_TILE_METERS * 0.5;
        entity->addComponent<YSort>();
        //entity->addComponent<Body>();
        entity->transform.position = hg::Vec3(pos[0] * MAP_TILE_METERS, (m_map->size[1] - pos[1]) * MAP_TILE_METERS, 0);

        state->mapResources.insert(entity->transform.position.resize<2>(), MAP_TILE_METERS, entity);

        if (state->mapResourceCounts.find(type) == state->mapResourceCounts.end()) {
            state->mapResourceCounts.insert(std::make_pair(type, 0));
        }

        state->mapResourceCounts[type]++;

        return true;
    }

    return false;
}


