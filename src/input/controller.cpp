#include "../../include/input/controller.h"
#include "../../include/realtime/motion.h"

namespace {
bool hasPieceAt(const GameSnapshot& snapshot, Position pos) {
    for (const MovingPiece& entry : snapshot.pieces) {
        if (entry.state != PieceState::CAPTURED && roundToSquare(entry.currentPos) == pos) {
            return true;
        }
    }
    return false;
}
}

Controller::Controller(BoardMapper& boardMapper, MoveRequestHandler onMoveRequested)
    : m_boardMapper(boardMapper), m_onMoveRequested(std::move(onMoveRequested)), m_selectedPosition(std::nullopt) {}

void Controller::updateSnapshot(const GameSnapshot& snapshot) {
    m_lastSnapshot = snapshot;
}

void Controller::handleInput(int x, int y) {
    if (!m_lastSnapshot) return; // no game state received yet

    auto targetPos = m_boardMapper.pixelWindowToCell(x, y);
    bool isInsideBoard = targetPos.row >= 0 && targetPos.row < m_lastSnapshot->height &&
                         targetPos.col >= 0 && targetPos.col < m_lastSnapshot->width;

    if (!m_selectedPosition.has_value()) {
        if (!isInsideBoard) return;
        if (hasPieceAt(*m_lastSnapshot, targetPos)) {
            m_selectedPosition = targetPos;
        }
    } else {
        if (!isInsideBoard) {
            m_selectedPosition = std::nullopt;
            return;
        }
        m_onMoveRequested(m_selectedPosition.value(), targetPos);
        m_selectedPosition = std::nullopt;
    }
}
