#pragma once
#include "../../include/input/board_mapper.h"
#include "../../include/model/Position.h"
#include <cmath>

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

Position BoardMapper::pixelWindowToCell(int windowX, int windowY) const {
    int col = static_cast<int>(std::floor((windowX - layout.offset.x) / layout.cellSize.width));
    int row = static_cast<int>(std::floor((windowY - layout.offset.y) / layout.cellSize.height));

    return Position{row, col};
}

PixelPoint BoardMapper::cellToPixel(const ExactPosition& pos, const CellSize& cellSize) {
    int x = static_cast<int>(pos.col * cellSize.width);
    int y = static_cast<int>(pos.row * cellSize.height);

    return PixelPoint{x, y};
}

PixelPoint BoardMapper::cellToPixelWindow(const ExactPosition& pos) const {
    int x = layout.offset.x + static_cast<int>(pos.col * layout.cellSize.width);
    int y = layout.offset.y + static_cast<int>(pos.row * layout.cellSize.height);

    return PixelPoint{x, y};
}

void BoardMapper::updateLayout(const BoardLayout& newLayout) {
    layout = newLayout;
}