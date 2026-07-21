#pragma once
#include <functional>
#include <vector>
#include "../Img.h"
#include "manage_element.h"
#include "renderable_element.h"

struct GameSnapshot;

class Canvas {
public:
    using ContentProvider = std::function<Img(const GameSnapshot&, int)>;

    Canvas(int windowWidth, int windowHeight, int channels);

    void registerElement(RenderableElement& element, ManageElement& layout, ContentProvider provider);

    void refreshAll(const GameSnapshot& snapshot, int elapsedMs);
    const cv::Mat& composeAll(int windowWidth, int windowHeight);

private:
    struct Slot {
        RenderableElement& element;
        ManageElement& layout;
        ContentProvider provider;
    };

    std::vector<Slot> slots;
    Img buffer;
};
