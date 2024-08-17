//
// Created by henry on 8/16/24.
//

#ifndef THERUNNER_RESOURCE_H
#define THERUNNER_RESOURCE_H

#include <hagame/core/component.h>

class Resource : public hg::Component {
public:

    std::string sprite;


protected:

    OBJECT_NAME(Resource)

};

#endif //THERUNNER_RESOURCE_H
