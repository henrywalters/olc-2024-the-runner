//
// Created by henry on 8/16/24.
//
#include "runtime.h"

#include "../systems/renderer.h"
#include "../systems/movement.h"

#include "../components/body.h"
#include "../components/player.h"

using namespace hg;
using namespace hg::graphics;

Runtime::Runtime(hg::graphics::Window *window):
    m_window(window)
{

}

void Runtime::onInit() {
    addSystem<Renderer>(m_window);
    addSystem<Movement>();

    auto player = entities.add();
    player->addComponent<Player>();
    player->addComponent<Body>();
}

void Runtime::onUpdate(double dt) {

}

