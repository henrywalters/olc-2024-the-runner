//
// Created by henry on 8/18/24.
//

#ifndef THERUNNER_TREE_H
#define THERUNNER_TREE_H
#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include "../common/tiles.h"

class TreeComponent : public hg::Component {
public:

    hg::Vec2i index;

protected:

    OBJECT_NAME(TreeComponent)

};

HG_COMPONENT(Game, TreeComponent)
HG_FIELD(TreeComponent, hg::Vec2i, index)

#endif //THERUNNER_TREE_H
