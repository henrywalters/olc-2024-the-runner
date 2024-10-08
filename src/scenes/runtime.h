//
// Created by henry on 8/16/24.
//

#ifndef THERUNNER_RUNTIME_H
#define THERUNNER_RUNTIME_H

#include <hagame/core/scene.h>
#include <hagame/graphics/window.h>
#include <hagame/ui/elements/label.h>
#include "../common/mapBuilder.h"

class Runtime : public hg::Scene {
public:

    Runtime(hg::graphics::Window* window);

    void startNewDay(bool addResourcesToInventory, bool clearInventory);
    void restart();

protected:

    void onInit() override;
    void onUpdate(double dt) override;

private:

    std::unique_ptr<MapBuilder> m_mapBuilder;

    hg::Entity* m_home;
    hg::Entity* m_player;

    hg::ui::Label* m_totalDisplay;
    hg::ui::Label* m_timeDisplay;
    hg::ui::Label* m_survivedDisplay;
    hg::Entity* m_menuScreen;
    hg::Entity* m_deathScreen;
    hg::Entity* m_survivedScreen;

    hg::graphics::Window* m_window;

    void addMenuScreen();
    void addDeathScreen();
    void addSurvivedScreen();
    void addHUD();

};

#endif //THERUNNER_RUNTIME_H
