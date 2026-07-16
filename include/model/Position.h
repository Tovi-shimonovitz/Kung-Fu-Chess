#pragma once
#include <string>
class Position
{
public:
    int row;
    int col;

    Position() = default;
    Position(int r, int c);
    bool operator==(const Position& other) const;
    bool operator<(const Position& other) const;
    std::string toString() const;
};
