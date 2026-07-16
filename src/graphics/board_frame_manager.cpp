#include "../../include/graphics/board_frame_manager.h"
#include <algorithm>

BoardFrameManager::BoardFrameManager(BoardMapper& mapper) : mapper(mapper) {}

Img BoardFrameManager::resizeToFit(const Img& boardFrame, int windowWidth, int windowHeight) {
    int boxWidth = windowWidth / 3;
    int boxHeight = windowHeight / 3;

    double scale = std::min(static_cast<double>(boxWidth) / boardFrame.get_mat().cols,
                             static_cast<double>(boxHeight) / boardFrame.get_mat().rows);

    return boardFrame.resized(static_cast<int>(boardFrame.get_mat().cols * scale),
                               static_cast<int>(boardFrame.get_mat().rows * scale));
}

BoardMapper::BoardLayout BoardFrameManager::computeLayout(const Img& scaled, int windowWidth, int windowHeight,
                                                           int cellsWide, int cellsHigh) {
    BoardMapper::BoardLayout layout;
    layout.offset = {
        (windowWidth - scaled.get_mat().cols) / 2,
        (windowHeight - scaled.get_mat().rows) / 2
    };
    layout.cellSize = {
        static_cast<double>(scaled.get_mat().cols) / cellsWide,
        static_cast<double>(scaled.get_mat().rows) / cellsHigh
    };
    return layout;
}

Img BoardFrameManager::compose(const Img& boardFrame, int windowWidth, int windowHeight,
                                int cellsWide, int cellsHigh) {
    Img scaled = resizeToFit(boardFrame, windowWidth, windowHeight);
    BoardMapper::BoardLayout layout = computeLayout(scaled, windowWidth, windowHeight, cellsWide, cellsHigh);
    mapper.updateLayout(layout);

    Img canvas = Img::blank(windowWidth, windowHeight, boardFrame.get_mat().channels());
    scaled.draw_on(canvas, layout.offset.x, layout.offset.y);

    return canvas;
}
