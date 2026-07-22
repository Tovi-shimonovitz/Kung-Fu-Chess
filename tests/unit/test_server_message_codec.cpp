#include <cassert>
#include <iostream>
#include <memory>
#include "../../include/network/protocol/server_message_codec.h"
#include "../../include/engine/GameEngine.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_round_trip_preserves_piece_fields() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({1, 2}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {1, 2}));
    board->addPiece({6, 4}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::PAWN, {6, 4}));
    engine.setBoard(std::move(board));

    GameSnapshot original = engine.getSnapshot();
    std::cout << "  original pieces: " << original.pieces.size() << std::endl;

    ServerRawMessage raw = ServerMessageCodec::toRaw(original);
    std::string text = ServerMessageCodec::serializeRaw(raw);
    std::cout << "  wire text: " << text << std::endl;

    ServerRawMessage roundTrippedRaw = ServerMessageCodec::parseRaw(text);
    GameSnapshot roundTripped = ServerMessageCodec::parseGameSnapshot(roundTrippedRaw);

    assert(roundTripped.width == original.width);
    assert(roundTripped.height == original.height);
    assert(roundTripped.isGameOver == original.isGameOver);
    assert(roundTripped.pieces.size() == original.pieces.size());

    for (size_t i = 0; i < original.pieces.size(); ++i) {
        const MovingPiece& a = original.pieces[i];
        const MovingPiece& b = roundTripped.pieces[i];
        std::cout << "  piece " << i << ": id=" << a.id << " color=" << static_cast<int>(a.color)
                  << "->" << static_cast<int>(b.color) << " kind=" << static_cast<int>(a.kind)
                  << "->" << static_cast<int>(b.kind) << " state=" << static_cast<int>(a.state)
                  << "->" << static_cast<int>(b.state) << std::endl;
        assert(a.id == b.id);
        assert(a.color == b.color);
        assert(a.kind == b.kind);
        assert(a.state == b.state);
        assert(a.currentPos.row == b.currentPos.row);
        assert(a.currentPos.col == b.currentPos.col);
    }

    log_test("test_round_trip_preserves_piece_fields");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING SERVER MESSAGE CODEC TESTS    " << std::endl;
    std::cout << "========================================" << std::endl;

    test_round_trip_preserves_piece_fields();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL SERVER MESSAGE CODEC TESTS EXECUTED" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
