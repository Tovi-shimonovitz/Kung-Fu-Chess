#pragma once
#include <vector>
#include "../Img.h"
#include "manage_element.h"
#include "renderable_element.h"

class Canvas {
public:
    Canvas(int windowWidth, int windowHeight, int channels);

    void registerElement(RenderableElement& element, ManageElement& layout);

    const cv::Mat& composeAll(int windowWidth, int windowHeight);

private:
    struct Slot {
        RenderableElement& element;
        ManageElement& layout;
    };

    std::vector<Slot> slots;
    Img buffer;
};
