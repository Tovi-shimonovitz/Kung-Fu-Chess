#pragma once
#include <optional>
#include "../model/Position.h"
#include "board_mapper.h"
#include "../engine/GameEngine.h"

class Controller {
public:
    Controller(GameEngine& engine);
    
    void handleInput(int x, int y);

private:
    GameEngine& m_engine;
    std::optional<Position> m_selectedPosition; 
};