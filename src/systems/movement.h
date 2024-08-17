//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_MOVEMENT_H
#define THERUNNER_MOVEMENT_H

#include <hagame/core/system.h>

#include "../components/body.h"
#include "../components/player.h"

class Movement : public hg::System {
public:

    void onUpdate(double dt) override;

};

#endif //THERUNNER_MOVEMENT_H
