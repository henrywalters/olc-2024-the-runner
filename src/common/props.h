//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_PROPS_H
#define THERUNNER_PROPS_H

#include <hagame/utils/enum.h>
#include <hagame/math/aliases.h>

ENUM(PropType)
ENUM_VALUE(PropType, Tree)
ENUM_VALUE(PropType, PalmTree)
ENUM_VALUE(PropType, Rock)

struct PropDef {
    PropType::type type;
    std::string group;
    float rarity;
    float minScale;
    float maxScale;
    hg::Vec2 origin;
    float colliderRadius;
};

const std::vector<PropDef> PROPS {
    PropDef{PropType::Tree, "trees", 0.05, 2, 6, hg::Vec2(0, -0.45), 0.11 },
    PropDef{PropType::PalmTree, "sand", 0.05, 2, 7, hg::Vec2(0, -0.45), 0.11},
    PropDef{PropType::Rock, "rocks", 0.15, 1, 5, hg::Vec2(0, -0.0), 0.3},
};

#endif //THERUNNER_RESOURCES_H
