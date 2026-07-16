#pragma once
#include "../model/Board.h"
#include "../engine/Result_structs.h" 

class BoardPrinter {
public:
    void print(const GameSnapshot& snapshot) const;

private:
    static char pieceToChar(const Piece& piece);
};