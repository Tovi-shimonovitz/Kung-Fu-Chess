#pragma once
#include <functional>
#include <optional>
#include "../model/Position.h"
#include "board_mapper.h"
#include "../engine/Result_structs.h"

class Controller {
public:
    using MoveRequestHandler = std::function<void(Position, Position)>;

    Controller(BoardMapper& boardMapper, MoveRequestHandler onMoveRequested);

    void updateSnapshot(const GameSnapshot& snapshot);
    void handleInput(int x, int y);

private:
    BoardMapper& m_boardMapper;
    MoveRequestHandler m_onMoveRequested;
    std::optional<Position> m_selectedPosition;
    std::optional<GameSnapshot> m_lastSnapshot;
};
