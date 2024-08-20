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
    std::string sprite;
    hg::Vec2 size;
};

const std::vector<ResourceDef> RESOURCES {
    ResourceDef {ResourceType::Stone, "stone", hg::Vec2(0.5, 0.5)},
};

#endif //THERUNNER_RESOURCES_H
