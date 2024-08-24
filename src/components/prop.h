//
// Created by henry on 8/16/24.
//

#ifndef THERUNNER_PROP_H
#define THERUNNER_PROP_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

#include "../common/props.h"

class Prop : public hg::Component {
public:

    int propIndex = 0;
    int spriteIndex = 0;

    PropDef getDef() const {
        return PROPS[propIndex];
    }

protected:

    OBJECT_NAME(Prop)

};

HG_COMPONENT(Game, Prop)
HG_FIELD(Prop, int, propIndex)

#endif //THERUNNER_RESOURCE_H
