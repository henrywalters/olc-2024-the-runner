//
// Created by henry on 8/16/24.
//

#ifndef THERUNNER_RESOURCE_H
#define THERUNNER_RESOURCE_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

#include "../common/resources.h"

class Resource : public hg::Component {
public:

    int resourceIndex = 0;

    ResourceDef getDef() const {
        return RESOURCES[resourceIndex];
    }

protected:

    OBJECT_NAME(Resource)

};

HG_COMPONENT(Game, Resource)
HG_FIELD(Resource, int, resourceIndex)

#endif //THERUNNER_RESOURCE_H
