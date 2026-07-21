#include <iostream>
#include <map>
#include "../../include/io/board_printer.h"
#include "../../include/realtime/motion.h"

char BoardPrinter::pieceToChar(PieceKind kind) {
    switch (kind) {
        case PieceKind::KING:   return 'K';
        case PieceKind::QUEEN:  return 'Q';
        case PieceKind::ROOK:   return 'R';
        case PieceKind::BISHOP: return 'B';
        case PieceKind::KNIGHT: return 'N';
        case PieceKind::PAWN:   return 'P';
        default:                return '?';
    }
}

void BoardPrinter::print(const GameSnapshot& snapshot) const {
    std::map<Position, const MovingPiece*> pieceMap;
    for (const MovingPiece& entry : snapshot.pieces) {
        pieceMap[roundToSquare(entry.currentPos)] = &entry;
    }

    const int BOARD_ROWS = snapshot.height;
    const int BOARD_COLS = snapshot.width;

    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            Position pos(r, c);
            auto it = pieceMap.find(pos);
            if (it == pieceMap.end()) {
                std::cout << ".";
            } else {
                const MovingPiece* entry = it->second;
                std::cout << (entry->color == PieceColor::WHITE ? 'w' : 'b')
                          << pieceToChar(entry->kind);
            }

            if (c < BOARD_COLS - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }
}
