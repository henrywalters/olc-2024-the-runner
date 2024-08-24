//
// Created by henry on 2/29/24.
//

#ifndef HABAN_GAMESTATE_H
#define HABAN_GAMESTATE_H

#include <hagame/utils/random.h>
#include <hagame/utils/enum.h>
#include <hagame/input/input.h>
#include <hagame/utils/variantMap.h>
#include <hagame/core/entityMap.h>
#include <hagame/graphics/resolution.h>

#include "../constants.h"
#include "../common/enums/actions.h"
#include "../common/persistentSettings.h"
#include "resources.h"
#include "../components/inventory.h"

ENUM(DebugLevel)
ENUM_VALUE(DebugLevel, Disabled)
ENUM_VALUE(DebugLevel, Light)
ENUM_VALUE(DebugLevel, Heavy)

enum class EventType {
    MoveTo,
    MoveBack,
};

using Event = std::variant<>;

// Access GameState via the singleton Get function
class GameState {
public:

    struct Settings {
        bool running = true;
        bool editing = false;
        bool useLighting = false;
    } settings;

    PersistentSettings persistentSettings;

    hg::EntityMap2D mapTiles;
    hg::EntityMap2D mapProps;
    hg::EntityMap2D mapResources;
    std::unordered_map<ResourceType::type, int> mapResourceCounts;

    bool paused = false;
    double elapsedTime = 0;
    float levelTime = 5;
    int daysPassed = 0;

    struct Time {
        int hour;
        int minute;

        std::string toString() const {
            return (hour < 10 ? "0" : "") + std::to_string(hour) + ":" + (minute < 10 ? "0" : "") + std::to_string(minute);
        }
    };

    hg::graphics::Color skyColor() const;
    Time getTime() const;

    hg::Publisher<EventType, Event> events;

    float homeRadius = maxBlocks().magnitude() / 8;
    bool atHome = false;
    hg::utils::enum_t debugLevel = DebugLevel::Light;
    hg::input::Actions<hg::utils::enum_t> input;
    hg::utils::Random random = hg::utils::Random(RANDOM_SEED);

    Inventory totalInventory;

    static GameState* Get();

};

#endif //HABAN_GAMESTATE_H
