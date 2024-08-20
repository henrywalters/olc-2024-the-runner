//
// Created by henry on 4/12/23.
//

#ifndef HGAMETEMPLATE_CONSTANTS_H
#define HGAMETEMPLATE_CONSTANTS_H

#define GAME_NAME "The Runner"
#define GAME_SIZE hg::graphics::HD
#define GAME_VERSION "0.0.1"

#define SPLASH_SCREEN "hg_studio"
#define SPLASH_FONT "8bit"

#define TREE_DENSITY 0.1

#define PIXELS_PER_METER 64
#define MAP_TILE_METERS 1

#define RANDOM_SEED 1

#define HEADLESS 0 // If true, no window will be opened
#define USE_IMGUI 1 // If true, IMGUI will be enabled and taken care of in game.cpp
#define USE_CONSOLE 0

constexpr float maxBlocks() {
    return std::max(GAME_SIZE[0], GAME_SIZE[1]) / PIXELS_PER_METER;
}

inline float distance(hg::Vec3 a, hg::Vec3 b) {
    return (b - a).resize<2>().magnitude();
}

inline float distance(hg::Vec2 a, hg::Vec2 b) {
    return (b - a).magnitude();
}

#endif //HGAMETEMPLATE_CONSTANTS_H
