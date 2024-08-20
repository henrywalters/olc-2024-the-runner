//
// Created by henry on 8/19/24.
//

#ifndef THERUNNER_MAPBUILDER_H
#define THERUNNER_MAPBUILDER_H

#include <hagame/graphics/image.h>
#include "gameState.h"
#include "tiles.h"
#include "resources.h"

class MapBuilder {
public:

    MapBuilder(hg::Scene* scene, hg::graphics::Image* map, hg::graphics::Image* labels);

    void build(hg::Vec3& playerPos);

private:

    hg::graphics::Image* m_map;
    hg::graphics::Image* m_labels;
    hg::Scene* m_scene;

    void addTile(hg::Entity* entity, hg::Vec2i pos, TileType::type type);

    bool spawnTree(hg::Vec2i pos);

    bool spawnResource(hg::Vec2i pos, ResourceType::type type);

    // Check if there is a tile around other than the current tile
    bool hasNonNeighbor(hg::Vec2i index, TileType::type type);
};

#endif //THERUNNER_MAPBUILDER_H
