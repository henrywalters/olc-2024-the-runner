//
// Created by henry on 8/20/24.
//

#ifndef THERUNNER_INVENTORY_H
#define THERUNNER_INVENTORY_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include "../common/resources.h"

class Inventory : public hg::Component {
public:

    void clear();
    void add(ResourceType::type type, int amount);
    int count(ResourceType::type type);

private:

    std::unordered_map<ResourceType::type, int> m_inventory;

};

#endif //THERUNNER_INVENTORY_H
