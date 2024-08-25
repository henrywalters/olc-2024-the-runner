//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_BODY_H
#define THERUNNER_BODY_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

class Body : public hg::Component {
public:

    float friction = 10.0;
    bool dynamic = true;
    hg::Vec3 velocity;
    hg::Vec3 acceleration;

protected:

    OBJECT_NAME(Body)

};

HG_COMPONENT(Physics, Body)
HG_FIELD(Body, float, friction)
HG_FIELD(Body, hg::Vec3, velocity)
HG_FIELD(Body, hg::Vec3, acceleration)


#endif //THERUNNER_BODY_H
