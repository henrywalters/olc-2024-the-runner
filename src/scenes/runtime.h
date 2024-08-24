//
// Created by henry on 8/16/24.
//

#ifndef THERUNNER_RUNTIME_H
#define THERUNNER_RUNTIME_H

#include <hagame/core/scene.h>
#include <hagame/graphics/window.h>
#include "../common/mapBuilder.h"

class Runtime : public hg::Scene {
public:

    Runtime(hg::graphics::Window* window);

    void startNewDay();

protected:

    void onInit() override;
    void onUpdate(double dt) override;

private:

    std::unique_ptr<MapBuilder> m_mapBuilder;

    hg::Entity* m_home;
    hg::Entity* m_player;

    hg::graphics::Window* m_window;

};

#endif //THERUNNER_RUNTIME_H
