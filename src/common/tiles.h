//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_TILES_H
#define THERUNNER_TILES_H

#include <hagame/utils/enum.h>
#include <hagame/math/aliases.h>
#include <hagame/graphics/color.h>

ENUM(TileType)
ENUM_VALUE(TileType, Grass)
ENUM_VALUE(TileType, Sand)
ENUM_VALUE(TileType, Clay)
ENUM_VALUE(TileType, Water)
ENUM_VALUE(TileType, Ice)

struct TileDef {
    TileType::type type;
    hg::Vec2i tileIndex;
    hg::graphics::Color lookupColor;
};

const std::vector<TileDef> TILES {
    TileDef {TileType::Grass, hg::Vec2i(0, 0), hg::graphics::Color::green()},
    TileDef {TileType::Sand, hg::Vec2i(1, 0), hg::graphics::Color::yellow()},
    TileDef { TileType::Clay, hg::Vec2i(1, 2), hg::graphics::Color::red()},
    TileDef{TileType::Water, hg::Vec2i(1, 1), hg::graphics::Color::blue()},
    TileDef{TileType::Ice, hg::Vec2i(0,2), hg::graphics::Color::magenta()},
};

inline bool getTileByColor(hg::graphics::Color color, int& index) {
    for (int i = 0; i < TILES.size(); i++) {
        if (TILES[i].lookupColor == color) {
            index = i;
            return true;
        }
    }
    return false;
}

#endif //THERUNNER_TILES_H
