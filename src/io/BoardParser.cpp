#pragma once
#include "../../include/io/BoardParser.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <cctype>
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

Board BoardParser::buildBoardFromGrid(const std::vector<std::vector<std::string>>& temp_grid, bool colorFirst) {
    if (temp_grid.empty()) {
        throw std::runtime_error("ERROR EMPTY_BOARD");
    }

    int rows = static_cast<int>(temp_grid.size());
    int cols = static_cast<int>(temp_grid[0].size());
    Board board(cols, rows);

    for (int r = 0; r < rows; ++r) {
        if (static_cast<int>(temp_grid[r].size()) != cols) {
            throw std::runtime_error("ERROR ROW_WIDTH_MISMATCH");
        }
        for (int c = 0; c < cols; ++c) {
            const std::string& token = temp_grid[r][c];
            if (token.empty() || token == ".") continue;
            if (token.length() != 2) {
                throw std::runtime_error("ERROR UNKNOWN_TOKEN");
            }
            char colorChar = colorFirst ? token[0] : token[1];
            char typeChar  = colorFirst ? token[1] : token[0];
            Position pos(r, c);
            auto piece = PieceFactory::createPiece(
                parseColor(static_cast<char>(std::tolower(colorChar))),
                parseType(typeChar),
                pos);
            board.addPiece(pos, std::move(piece));
        }
    }
    return board;
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

    return buildBoardFromGrid(temp_grid, /*colorFirst=*/true);
}

Board BoardParser::parseFromCsv(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("ERROR CANNOT_OPEN_FILE");
    }

    std::vector<std::vector<std::string>> temp_grid;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        temp_grid.push_back(row);
    }

    return buildBoardFromGrid(temp_grid, /*colorFirst=*/false);
}