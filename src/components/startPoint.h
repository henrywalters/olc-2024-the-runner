//
// Created by henry on 8/21/24.
//

#ifndef THERUNNER_STARTPOINT_H
#define THERUNNER_STARTPOINT_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

class StartPoint : public hg::Component {
public:

    hg::Vec3 position;
    float radius;

protected:

    OBJECT_NAME(StartPoint)


};

#endif //THERUNNER_STARTPOINT_H
