#pragma once
#include "../../include/input/controller.h"
#include "../../include/engine/Result_structs.h"
#include <iostream>


Controller::Controller(GameEngine& engine, BoardMapper& boardMapper)
    : m_engine(engine), m_boardMapper(boardMapper), m_selectedPosition(std::nullopt) {}

void Controller::handleInput(int x, int y) {

    auto targetPos = m_boardMapper.pixelWindowToCell(x, y);
    const auto& layout = m_boardMapper.getLayout();
    std::cout << "[click] x=" << x << " y=" << y << " -> row=" << targetPos.row << " col=" << targetPos.col
               << " hasPiece=" << m_engine.hasPieceAt(targetPos)
               << " | layout offset=(" << layout.offset.x << "," << layout.offset.y
               << ") cellSize=(" << layout.cellSize.width << "," << layout.cellSize.height << ")" << std::endl;
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