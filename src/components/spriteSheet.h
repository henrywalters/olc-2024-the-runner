//
// Created by henry on 6/23/24.
//

#ifndef HABAN_SPRITESHEET_H
#define HABAN_SPRITESHEET_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

class SpriteSheetComponent : public hg::Component {
public:
    hg::Vec2 size;
    std::string texture;
    std::string spriteSheet;
    std::string spriteGroup;

protected:

    OBJECT_NAME(SpriteSheetComponent);
};

HG_COMPONENT(Graphics, SpriteSheetComponent)
HG_FIELD(SpriteSheetComponent, hg::Vec2, size);
HG_FIELD(SpriteSheetComponent, std::string, texture)
HG_FIELD(SpriteSheetComponent, std::string, spriteSheet)
HG_FIELD(SpriteSheetComponent, std::string, spriteGroup)


#endif //HABAN_SPRITESHEET_H
