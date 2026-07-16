#pragma once
#include <string>
#include "Piece_enums.h" 
#include "Position.h"

static int globalIdCounter = 0;
class Piece {
public:
    const std::string id;
    const PieceColor color;
    const PieceKind kind;
    Position position;
    PieceState state;

   
    Piece(PieceColor c, PieceKind k) 
    : id(std::to_string(++globalIdCounter)), 
      color(c), 
      kind(k), 
      position(0, 0),                       
      state(PieceState::IDLE)              
{}
    void setPosition(const Position& pos);

    std::string toString() const;
};
