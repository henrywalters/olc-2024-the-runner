//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_BODY_H
#define THERUNNER_BODY_H

#include <hagame/core/component.h>

class Body : public hg::Component {
public:

    float friction = 1.0;
    hg::Vec3 velocity;
    hg::Vec3 acceleration;

};


#endif //THERUNNER_BODY_H
