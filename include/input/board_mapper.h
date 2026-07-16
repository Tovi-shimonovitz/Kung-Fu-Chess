#pragma once
#include "../model/Position.h"
#include "../realtime/motion.h"

inline constexpr int CELL_SIZE = 100;

struct PixelPoint {
    int x;
    int y;
};

struct CellSize {
    double width;
    double height;
};

class BoardMapper {
public:
      static Position pixelToCell(int pixelX, int pixelY);
      static PixelPoint cellToPixel(const ExactPosition& pos, const CellSize& cellSize);
};