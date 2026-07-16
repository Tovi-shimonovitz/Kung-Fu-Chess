#pragma once
#include "../../include/engine/GameEngine.h"
#include "../../include/model/Board.h"
#include "../../include/rules/RuleEngine.h"
#include "../../include/realtime/real_time_arbiter.h"
#include "../../include/engine/Result_structs.h"
#include <algorithm>
#include <unordered_map>

void GameEngine::setBoard(std::unique_ptr<Board> newBoard) {
    board = std::move(newBoard); 
}

GameEngine::GameEngine() :
    board(),
    ruleEngine(std::make_unique<RuleEngine>()),
    arbiter(std::make_unique<RealTimeArbiter>())
{}

WaitResult GameEngine::wait(int ms) {
    if (gameState.isGameOver()) {
            return {false, "game_over"};
    }

   WaitResult result = arbiter->advanceTime(ms, *board);
    if(result.message.find("king_eaten") != std::string::npos) {
        gameState.markGameOver();
    }

    for (const auto& [id, observer] : waitObservers) {
        observer(ms);
    }

    return result;
}

WaitObserverId GameEngine::addWaitObserver(WaitObserver observer) {
    WaitObserverId id = nextWaitObserverId++;
    waitObservers.emplace_back(id, std::move(observer));
    return id;
}

void GameEngine::removeWaitObserver(WaitObserverId id) {
    waitObservers.erase(
        std::remove_if(waitObservers.begin(), waitObservers.end(),
                        [id](const auto& entry) { return entry.first == id; }),
        waitObservers.end());
}

MoveResult GameEngine::requestMove(Position src, Position dst) {
    if (gameState.isGameOver()) {
        return {false, "game_over"};
    }
    auto piece = board->getPieceAt(src);
    if (!piece) {
        return {false, "no_piece_at_source"};
    }

    if (piece->state == PieceState::MOVING || piece->state == PieceState::COOLDOWN) {
        return {false, "motion_in_progress"};
    }

    MoveValidation validation = ruleEngine->validateMove(*board, src, dst);
    if (validation.reason != MoveStatus::OK) {
        return {false, "illegal_piece_move"};
    }

    piece->state = PieceState::MOVING;
    arbiter->startMotion(piece, src, dst);

    return {true, "move_started"};

}

bool GameEngine::hasPieceAt(Position pos) const {
    return board->getPieceAt(pos) != nullptr;
}

GameSnapshot GameEngine::getSnapshot() const {
    GameSnapshot snapshot;
    snapshot.isGameOver = this->gameState.isGameOver();
    snapshot.height = board->height;
    snapshot.width = board->width;

    std::unordered_map<Piece*, ExactPosition> exactPositionOverride;
    for (const Motion& motion : arbiter->activeMotionsView()) {
        exactPositionOverride[motion.piece] = motion.currentPosition();
    }

    for (Piece* piece : board->getAllPieces()) {
        auto found = exactPositionOverride.find(piece);
        ExactPosition currentPos = (found != exactPositionOverride.end())
            ? found->second
            : ExactPosition{static_cast<double>(piece->position.row), static_cast<double>(piece->position.col)};

        snapshot.pieces.push_back(MovingPiece{piece, currentPos});
    }

    return snapshot;
}


