#pragma once
#include <functional>
#include <optional>
#include "../model/Position.h"
#include "../engine/Result_structs.h"

class Controller {
public:
    using MoveRequestHandler = std::function<void(Position, Position)>;

    explicit Controller(MoveRequestHandler onMoveRequested);

    void updateSnapshot(const GameSnapshot& snapshot);
    void handleInput(Position targetPos);

private:
    MoveRequestHandler m_onMoveRequested;
    std::optional<Position> m_selectedPosition;
    std::optional<GameSnapshot> m_lastSnapshot;
};
