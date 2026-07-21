#include "graphics/game_view_bindings.h"
#include "graphics/board/board_renderer.h"
#include "graphics/window/canvas.h"

void bindBoardElement(Canvas& canvas, BoardRenderer& renderer, RenderableElement& boardElement, ManageElement& layout) {
    canvas.registerElement(boardElement, layout, [&renderer](const GameSnapshot& snapshot, int elapsedMs) {
        return renderer.render(snapshot, elapsedMs);
    });
}
