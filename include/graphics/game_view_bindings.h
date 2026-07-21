#pragma once
class Canvas;
class BoardRenderer;
class RenderableElement;
class ManageElement;

void bindBoardElement(Canvas& canvas, BoardRenderer& renderer, RenderableElement& boardElement, ManageElement& layout);
// בעתיד: void bindMoveTableElement(...);
