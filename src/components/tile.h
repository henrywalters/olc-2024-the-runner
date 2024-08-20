//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_TILE_H
#define THERUNNER_TILE_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include "../common/tiles.h"

class MapTile : public hg::Component {
public:

    int tileIndex = 0;

    TileDef getDef() const {
        return TILES[tileIndex];
    }

protected:

    OBJECT_NAME(MapTile)

};

HG_COMPONENT(Game, MapTile)
HG_FIELD(MapTile, int, tileIndex)

#endif //THERUNNER_TILE_H
