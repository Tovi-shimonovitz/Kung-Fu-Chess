#pragma once
#include "../Img.h"

struct PositionedImg {
    Img image;
    int x;
    int y;
};

class ManageElement {
public:
    virtual ~ManageElement() = default;

    virtual PositionedImg prepareForDisplay(const Img& content, int windowWidth, int windowHeight) const = 0;
};
