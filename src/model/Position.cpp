#pragma once
#include "../../include//model/Position.h"

Position::Position(int r, int c) : row(r), col(c) {}

bool Position::operator==(const Position& other) const {
    return (this->row == other.row) && (this->col == other.col);
}

bool Position::operator<(const Position& other) const {
    if (this->row != other.row) {
        return this->row < other.row;
    }
    return this->col < other.col;
}

std::string Position::toString() const {
    return "(row: " + std::to_string(row) + ", col: " + std::to_string(col) + ")";
}
