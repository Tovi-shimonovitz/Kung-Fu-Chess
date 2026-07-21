#include "../../../include/graphics/board/board_renderer.h"
#include "../../../include/input/board_mapper.h"
#include <unordered_set>

BoardRenderer::BoardRenderer(std::string boardImagePath, SpriteRepository& spriteRepository)
    : boardImagePath(std::move(boardImagePath)), spriteRepository(spriteRepository) {
    boardImage.read(this->boardImagePath);
}

PieceAnimator& BoardRenderer::animatorFor(const std::string& id, PieceColor color, PieceKind kind) {
    auto found = animators.find(id);
    if (found != animators.end()) {
        return found->second;
    }

    PieceAnimationSet& animationSet = spriteRepository.get(color, kind);
    auto inserted = animators.emplace(id, PieceAnimator(animationSet));
    return inserted.first->second;
}

void BoardRenderer::removeStaleAnimators(const GameSnapshot& snapshot) {
    std::unordered_set<std::string> stillOnBoard;
    for (const MovingPiece& entry : snapshot.pieces) {
        stillOnBoard.insert(entry.id);
    }

    for (auto it = animators.begin(); it != animators.end(); ) {
        if (stillOnBoard.find(it->first) == stillOnBoard.end()) {
            it = animators.erase(it);
        } else {
            ++it;
        }
    }
}

Img BoardRenderer::render(const GameSnapshot& snapshot, int elapsedMs) {
    removeStaleAnimators(snapshot);

    Img frame = boardImage.clone();

    CellSize realCellSize{
        static_cast<double>(frame.get_mat().cols) / snapshot.width,
        static_cast<double>(frame.get_mat().rows) / snapshot.height
    };

    for (const MovingPiece& entry : snapshot.pieces) {
        if (entry.state == PieceState::CAPTURED) {
            continue;
        }

        PieceAnimator& animator = animatorFor(entry.id, entry.color, entry.kind);
        animator.syncToGameState(entry.state);
        animator.advance(elapsedMs);

        PixelPoint pixel = BoardMapper::cellToPixel(entry.currentPos, realCellSize);

        Img& spriteFrame = animator.currentFrame();
        int offsetX = static_cast<int>((realCellSize.width - spriteFrame.get_mat().cols) / 2);
        int offsetY = static_cast<int>(realCellSize.height) - spriteFrame.get_mat().rows;

        spriteFrame.draw_on(frame, pixel.x + offsetX, pixel.y + offsetY);
    }

    return frame;
}
