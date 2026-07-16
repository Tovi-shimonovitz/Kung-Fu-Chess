#include "../../include/graphics/board_renderer.h"
#include "../../include/input/board_mapper.h"
#include "../../include/model/Piece.h"
#include <unordered_set>

BoardRenderer::BoardRenderer(std::string boardImagePath, SpriteRepository& spriteRepository)
    : boardImagePath(std::move(boardImagePath)), spriteRepository(spriteRepository) {
    boardImage.read(this->boardImagePath);
}

PieceAnimator& BoardRenderer::animatorFor(Piece* piece) {
    auto found = animators.find(piece);
    if (found != animators.end()) {
        return found->second;
    }

    PieceAnimationSet& animationSet = spriteRepository.get(piece->color, piece->kind);
    auto inserted = animators.emplace(piece, PieceAnimator(animationSet));
    return inserted.first->second;
}

void BoardRenderer::removeStaleAnimators(const GameSnapshot& snapshot) {
    std::unordered_set<Piece*> stillOnBoard;
    for (const MovingPiece& entry : snapshot.pieces) {
        stillOnBoard.insert(entry.piece);
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
        Piece* piece = entry.piece;
        if (piece->state == PieceState::CAPTURED) {
            continue;
        }

        PieceAnimator& animator = animatorFor(piece);
        animator.syncToGameState(piece->state);
        animator.advance(elapsedMs);

        PixelPoint pixel = BoardMapper::cellToPixel(entry.currentPos, realCellSize);

        Img& spriteFrame = animator.currentFrame();
        int offsetX = static_cast<int>((realCellSize.width - spriteFrame.get_mat().cols) / 2);
        int offsetY = static_cast<int>(realCellSize.height) - spriteFrame.get_mat().rows;

        spriteFrame.draw_on(frame, pixel.x + offsetX, pixel.y + offsetY);
    }

    return frame;
}
