#pragma once
#include <optional>
#include "../model/Position.h"
#include "board_mapper.h"
#include "../engine/GameEngine.h"

class Controller {
public:
    Controller(GameEngine& engine, BoardMapper& boardMapper);

    void handleInput(int x, int y);

private:
    GameEngine& m_engine;
    BoardMapper& m_boardMapper;
    std::optional<Position> m_selectedPosition;
};