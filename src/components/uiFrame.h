//
// Created by henry on 8/20/24.
//

#ifndef THERUNNER_UIFRAME_H
#define THERUNNER_UIFRAME_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include "../common/ui.h"

class UIFrame : public hg::Component {
public:

    bool debug = true;

    UIFrame(hg::Vec2i size):
        frame(hg::Rect(hg::Vec2::Zero(), size.cast<float>()))
    {}

    hg::ui::Frame frame;

protected:

    OBJECT_NAME(UIFrame)

};

#endif //THERUNNER_UIFRAME_H
