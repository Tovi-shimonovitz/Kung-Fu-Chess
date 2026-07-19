#include "../../../include/graphics/window/canvas.h"

Canvas::Canvas(int windowWidth, int windowHeight, int channels)
    : buffer(Img::blank(windowWidth, windowHeight, channels)) {}

void Canvas::registerElement(RenderableElement& element, ManageElement& layout) {
    slots.push_back({element, layout});
}

const cv::Mat& Canvas::composeAll(int windowWidth, int windowHeight) {
    if (windowWidth != buffer.get_mat().cols || windowHeight != buffer.get_mat().rows) {
        buffer = Img::blank(windowWidth, windowHeight, buffer.get_mat().channels());
    } else {
        buffer.clear();
    }

    for (auto& slot : slots) {
        PositionedImg positioned = slot.layout.prepareForDisplay(
            slot.element.currentContent(), windowWidth, windowHeight);
        positioned.image.draw_on(buffer, positioned.x, positioned.y);
    }

    return buffer.get_mat();
}
