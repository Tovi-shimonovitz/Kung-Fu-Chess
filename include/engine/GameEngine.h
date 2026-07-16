#pragma once
#include "Result_structs.h"
#include "GameState.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "../model/Board.h"
#include "../rules/RuleEngine.h"
#include "../realtime/real_time_arbiter.h"

using WaitObserver = std::function<void(int elapsedMs)>;

class GameEngine {
public:
    GameEngine();

    MoveResult requestMove(Position src, Position dst);

    WaitResult wait(int ms);

    GameSnapshot getSnapshot() const;

    bool hasPieceAt(Position pos) const;
    void setBoard(std::unique_ptr<Board> newBoard);

    void addWaitObserver(WaitObserver observer);

private:
    std::unique_ptr<Board> board;
    std::unique_ptr<RuleEngine> ruleEngine;
    std::unique_ptr<RealTimeArbiter> arbiter;

    GameState gameState;
    std::vector<WaitObserver> waitObservers;
};