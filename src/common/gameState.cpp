//
// Created by henry on 2/29/24.
//
#include <memory>
#include "gameState.h"

GameState *GameState::Get() {
    static std::unique_ptr<GameState> gameState;
    if (!gameState) {
        gameState = std::make_unique<GameState>();
    }
    return gameState.get();
}

hg::graphics::Color GameState::skyColor() const {
    float brightness = (std::cos(elapsedTime * hg::math::PI / (levelTime / 2) + hg::math::PI) + 1) / 2;
    return hg::graphics::Color(brightness, brightness, brightness, 1.0f);
}

GameState::Time GameState::getTime() const {
    float scale = 3600 * 24 / levelTime / 1000;
    int seconds = (float) ((int) (elapsedTime * 1000) % (int) (levelTime * 1000)) * scale;
    int hours = seconds / 3600;
    int minutes = (seconds - hours * 3600) / 60;
    return Time {hours, minutes};
}

