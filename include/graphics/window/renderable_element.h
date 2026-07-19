#pragma once
#include "../Img.h"

class RenderableElement {
public:
    void updateContent(Img newContent) { content = std::move(newContent); }
    const Img& currentContent() const { return content; }

private:
    Img content;
};
