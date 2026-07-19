#pragma once
#include "../Img.h"
#include "../window/manage_element.h"
#include "../../input/board_mapper.h"

class BoardFrameManager : public ManageElement {
public:
    BoardFrameManager(BoardMapper& mapper, int cellsWide, int cellsHigh);

    PositionedImg prepareForDisplay(const Img& content, int windowWidth, int windowHeight) const override;

private:
    static Img resizeToFit(const Img& boardFrame, int windowWidth, int windowHeight);
    static BoardMapper::BoardLayout computeLayout(const Img& scaled, int windowWidth, int windowHeight,
                                                   int cellsWide, int cellsHigh);

    BoardMapper& mapper;
    int cellsWide;
    int cellsHigh;
};
