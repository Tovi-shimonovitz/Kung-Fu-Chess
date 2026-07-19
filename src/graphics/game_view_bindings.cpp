#include "graphics/game_view_bindings.h"
#include "engine/GameEngine.h"
#include "graphics/board/board_renderer.h"
#include "graphics/window/renderable_element.h"

void bindBoardElement(GameEngine& engine, BoardRenderer& renderer, RenderableElement& boardElement) {
    engine.addWaitObserver([&engine, &renderer, &boardElement](int elapsedMs) {
        Img raw = renderer.render(engine.getSnapshot(), elapsedMs);
        boardElement.updateContent(std::move(raw));
    });
}
