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
      struct BoardLayout {
          PixelPoint offset;   
          CellSize cellSize;    
      };

      static Position pixelToCell(int pixelX, int pixelY);
      static PixelPoint cellToPixel(const ExactPosition& pos, const CellSize& cellSize);

      Position pixelWindowToCell(int windowX, int windowY) const;
      PixelPoint cellToPixelWindow(const ExactPosition& pos) const;
      void updateLayout(const BoardLayout& newLayout);

private:
      BoardLayout layout{ {0, 0}, {static_cast<double>(CELL_SIZE), static_cast<double>(CELL_SIZE)} };
};