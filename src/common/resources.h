//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_RESOURCES_H
#define THERUNNER_RESOURCES_H

#include <hagame/utils/enum.h>
#include <hagame/math/aliases.h>

ENUM(ResourceType)
ENUM_VALUE(ResourceType, Wood)
ENUM_VALUE(ResourceType, Stone)
ENUM_VALUE(ResourceType, Wheat)

struct ResourceDef {
    ResourceType::type type;
    hg::Vec2i tileIndex;
    float rarity;
};

const std::vector<ResourceDef> RESOURCES {
    ResourceDef{ResourceType::Wood, hg::Vec2i(0, 0), 0.05},
    ResourceDef {ResourceType::Stone, hg::Vec2i(1, 0), 0.25},
    ResourceDef {ResourceType::Wheat, hg::Vec2i(2, 0), 0.15},
};

#endif //THERUNNER_RESOURCES_H
