#pragma once
#include "../../include/input/board_mapper.h"
#include "../../include/model/Position.h"

namespace {
int floorDiv(int value) {
    int quotient = value / CELL_SIZE;
    if (value % CELL_SIZE != 0 && value < 0) {
        --quotient;
    }
    return quotient;
}
}

Position BoardMapper::pixelToCell(int pixelX, int pixelY) {
    int col = floorDiv(pixelX);
    int row = floorDiv(pixelY);

    return Position{row, col};
}

PixelPoint BoardMapper::cellToPixel(const ExactPosition& pos, const CellSize& cellSize) {
    int x = static_cast<int>(pos.col * cellSize.width);
    int y = static_cast<int>(pos.row * cellSize.height);

    return PixelPoint{x, y};
}