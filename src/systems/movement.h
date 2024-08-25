//
// Created by henry on 8/17/24.
//

#ifndef THERUNNER_MOVEMENT_H
#define THERUNNER_MOVEMENT_H

#include <hagame/core/system.h>
#include <hagame/core/entityMap.h>

#include "../components/body.h"
#include "../components/player.h"

class Movement : public hg::System {
public:

    // Make sure to call this after generating the map!
    void bakeStaticColliders();

    void onUpdate(double dt) override;

private:

    hg::EntityMap2D m_staticColliderMap;
    hg::EntityMap2D m_colliderMap;

    void handleCollision(hg::math::Ray movementRay, hg::Entity* entity, Body* body, hg::Entity* neighbor);
};

#endif //THERUNNER_MOVEMENT_H
