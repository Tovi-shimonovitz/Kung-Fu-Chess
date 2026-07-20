#pragma once
#include "../model/Position.h"
#include "../model/Piece_enums.h"

class Piece;

struct MoveEvent          { Position source; Position target; Piece* piece; };
struct PieceCapturedEvent { PieceKind capturedKind; PieceColor capturedColor; PieceColor byColor; };
struct GameStartedEvent   {};
struct GameEndedEvent     { PieceColor winner; };
