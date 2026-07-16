#pragma once
#include "../../include/input/controller.h"
#include "../../include/engine/Result_structs.h"


Controller::Controller(GameEngine& engine) 
    : m_engine(engine), m_selectedPosition(std::nullopt) {}

void Controller::handleInput(int x, int y) {
   
    auto targetPos = BoardMapper::pixelToCell(x, y);
    auto cur_snapshot = m_engine.getSnapshot();

    bool isInsideBoard = targetPos.row >= 0 && targetPos.row < cur_snapshot.height && 
                     targetPos.col >= 0 && targetPos.col < cur_snapshot.width;

    if (!m_selectedPosition.has_value()) {
        if (!isInsideBoard) return; 

        if (m_engine.hasPieceAt(targetPos)) {
            m_selectedPosition = targetPos; 
        }
    } else {
        if (!isInsideBoard) {
            m_selectedPosition = std::nullopt; 
            return;
        }

        m_engine.requestMove(m_selectedPosition.value(), targetPos);
        
        m_selectedPosition = std::nullopt;
    }
}