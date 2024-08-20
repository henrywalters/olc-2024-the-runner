//
// Created by henry on 8/18/24.
//

#ifndef THERUNNER_YSORT_H
#define THERUNNER_YSORT_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include "../common/tiles.h"

class YSort : public hg::Component {
public:

    hg::Vec2 sortPoint;

protected:

    OBJECT_NAME(YSort)

};

HG_COMPONENT(Game, YSort)
HG_FIELD(YSort, hg::Vec2, sortPoint)

#endif //THERUNNER_YSORT_H
