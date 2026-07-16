#pragma once
#include "../../include/io/BoardParser.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"
#include <sstream>
#include <stdexcept>
#include "../../include/model/Board.h"

 PieceColor BoardParser::parseColor(char c) {
    if (c == 'w') return PieceColor::WHITE;
    if (c == 'b') return PieceColor::BLACK;
    throw std::runtime_error("ERROR UNKNOWN_TOKEN");
}
 PieceKind BoardParser::parseType(char t) {
    switch (t) {
    case 'K': return PieceKind::KING;
    case 'Q': return PieceKind::QUEEN;
    case 'R': return PieceKind::ROOK;
    case 'B': return PieceKind::BISHOP;
    case 'N': return PieceKind::KNIGHT;
    case 'P': return PieceKind::PAWN;
    default: throw std::runtime_error("ERROR UNKNOWN_TOKEN");
    }
}

Board BoardParser::parseFromTextLines(std::istream& input) {
    if(input.peek() == EOF){
        throw std::runtime_error("ERROR EMPTY_BOARD");
    }
    std::vector<std::vector<std::string>> temp_grid;
    std::string line;

    while (std::getline(input, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> row;
        while (ss >> token) {
            if (token != "." && token.length() != 2) throw std::runtime_error("ERROR UNKNOWN_TOKEN");
            row.push_back(token);
        }
        if (!row.empty()) temp_grid.push_back(row);

    }

    int rows = static_cast<int>(temp_grid.size());
    int cols = static_cast<int>(temp_grid[0].size());
    Board board(cols, rows);

    for (int r = 0; r < rows; ++r) {
        if(temp_grid[r].size() != cols){
            throw std::runtime_error("ERROR ROW_WIDTH_MISMATCH");            
        }
        for (int c = 0; c < cols; ++c) {
            if (temp_grid[r][c] != ".") {
                char colorChar = temp_grid[r][c][0];
                char typeChar = temp_grid[r][c][1];
                Position pos(r, c);
                auto piece = PieceFactory::createPiece(parseColor(colorChar), parseType(typeChar), pos);
                board.addPiece(pos, std::move(piece));
            }
        }
    }
    return board;
}