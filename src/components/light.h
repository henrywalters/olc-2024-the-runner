//
// Created by henry on 8/25/24.
//

#ifndef THERUNNER_LIGHT_H
#define THERUNNER_LIGHT_H

#include <hagame/core/entity.h>

class Light : public hg::Component {
public:

    hg::graphics::Color color = hg::graphics::Color::white();
    float attenuation = 10;
    float attenuationSq = 10;
    float size = 10.0;

protected:

    OBJECT_NAME(Light)
};

HG_COMPONENT(Game, Light)

#endif //THERUNNER_LIGHT_H
