#pragma once
#include <istream>
#include <string>
#include <vector>
#include "../model/Board.h"
#include "../model/Piece_enums.h"
#include "../model/Position.h"

class BoardParser {
public:
    BoardParser() = default;
    static Board parseFromTextLines(std::istream& input);

private:
    static PieceColor parseColor(char c);
    static PieceKind parseType(char t);
    //static bool isTokenValid(const std::string& token);
};
