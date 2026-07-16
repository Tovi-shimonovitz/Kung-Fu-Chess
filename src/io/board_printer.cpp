#pragma once
#include <iostream>
#include "../../include/model/Board.h"
#include "../../include/io/board_printer.h"
#include <iostream>
#include <map>

     char BoardPrinter::pieceToChar(const Piece& piece) {
        switch (piece.kind) {
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
    std::map<Position, Piece*> pieceMap;
    for (const MovingPiece& entry : snapshot.pieces) {
        pieceMap[entry.piece->position] = entry.piece;
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
                auto piece = it->second;
                std::cout << (piece->color == PieceColor::WHITE ? 'w' : 'b')
                          << pieceToChar(*piece);
            }
            
            if (c < BOARD_COLS - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }
}