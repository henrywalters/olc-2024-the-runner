//
// Created by henry on 7/21/24.
//

#ifndef HABAN_SETTINGS_H
#define HABAN_SETTINGS_H

#include <hagame/core/scene.h>
#include <hagame/graphics/window.h>
#include <hagame/ui/elements/container.h>
#include <hagame/ui/elements/checkbox.h>
#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/mesh.h>
#include <hagame/graphics/textBuffer.h>
#include <hagame/ui/frame.h>

class Settings : public hg::Scene {
public:

    enum class RenderMode {
        Color,
    };

    Settings(hg::graphics::Window* window);

protected:

    void onInit() override;
    void onAfterUpdate() override;
    void onUpdate(double dt) override;

private:

    bool m_devMode;
    bool m_vsync;

    hg::ui::Checkbox* m_devModeCbox;
    hg::ui::Checkbox* m_vsyncCbox;

    hg::graphics::BatchRenderer m_batchRenderer;
    hg::graphics::RenderPasses<RenderMode> m_renderPasses;

    hg::graphics::primitives::Quad m_quad;
    hg::graphics::MeshInstance m_mesh;

    hg::graphics::Window* m_window;
    hg::ui::Container m_uiRoot;

    hg::ui::Frame m_menu;

    void resize();

};

#endif //HABAN_MAINMENU_H
