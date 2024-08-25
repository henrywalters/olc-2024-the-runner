//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_PLAYER_H
#define THERUNNER_PLAYER_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

class Player : public hg::Component {
public:

    float acceleration = 100.0;

protected:

    OBJECT_NAME(Player)

};

HG_COMPONENT(Game, Player)
HG_FIELD(Player, float, acceleration);

#endif //THERUNNER_PLAYER_H
