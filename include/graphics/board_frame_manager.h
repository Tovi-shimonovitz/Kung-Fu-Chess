#pragma once
#include "Img.h"
#include "../input/board_mapper.h"

class BoardFrameManager {
public:
    explicit BoardFrameManager(BoardMapper& mapper);

    Img compose(const Img& boardFrame, int windowWidth, int windowHeight,
                int cellsWide, int cellsHigh);

private:
    static Img resizeToFit(const Img& boardFrame, int windowWidth, int windowHeight);
    static BoardMapper::BoardLayout computeLayout(const Img& scaled, int windowWidth, int windowHeight,
                                                   int cellsWide, int cellsHigh);

    BoardMapper& mapper;
};
