//
// Created by henry on 8/20/24.
//
#include "inventory.h"

void Inventory::clear() {
    m_inventory.clear();
}

void Inventory::add(ResourceType::type type, int amount) {
    if (m_inventory.find(type) == m_inventory.end()) {
        m_inventory.insert(std::make_pair(type, 0));
    }
    m_inventory[type] += amount;
}

int Inventory::count(ResourceType::type type) {
    return m_inventory.find(type) == m_inventory.end() ? 0 : m_inventory[type];
}
