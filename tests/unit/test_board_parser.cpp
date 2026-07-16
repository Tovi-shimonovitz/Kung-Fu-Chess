#include <cassert>
#include <iostream>
#include <sstream>
#include "../../include/io/BoardParser.h"
#include "../../include/model/Piece_enums.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_parse_simple_board() {
    std::istringstream input(
        "wR . . .\n"
        ". bK . .\n"
    );

    Board board = BoardParser::parseFromTextLines(input);

    Piece* rook = board.getPieceAt(Position(0, 0));
    assert(rook != nullptr);
    assert(rook->color == PieceColor::WHITE);
    assert(rook->kind == PieceKind::ROOK);

    Piece* king = board.getPieceAt(Position(1, 1));
    assert(king != nullptr);
    assert(king->color == PieceColor::BLACK);
    assert(king->kind == PieceKind::KING);

    assert(board.getPieceAt(Position(0, 1)) == nullptr);

    log_test("test_parse_simple_board");
}

void test_parse_empty_input_throws() {
    std::istringstream input("");
    bool threw = false;
    try {
        BoardParser::parseFromTextLines(input);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);
    log_test("test_parse_empty_input_throws");
}

void test_parse_unknown_token_length_throws() {
    std::istringstream input("wRo .\n");
    bool threw = false;
    try {
        BoardParser::parseFromTextLines(input);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);
    log_test("test_parse_unknown_token_length_throws");
}

void test_parse_unknown_color_throws() {
    std::istringstream input("xR .\n");
    bool threw = false;
    try {
        BoardParser::parseFromTextLines(input);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);
    log_test("test_parse_unknown_color_throws");
}

void test_parse_unknown_piece_type_throws() {
    std::istringstream input("wZ .\n");
    bool threw = false;
    try {
        BoardParser::parseFromTextLines(input);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);
    log_test("test_parse_unknown_piece_type_throws");
}

void test_parse_all_piece_kinds() {
    std::istringstream input("wK wQ wR wB wN wP\n");
    Board board = BoardParser::parseFromTextLines(input);

    assert(board.getPieceAt(Position(0, 0))->kind == PieceKind::KING);
    assert(board.getPieceAt(Position(0, 1))->kind == PieceKind::QUEEN);
    assert(board.getPieceAt(Position(0, 2))->kind == PieceKind::ROOK);
    assert(board.getPieceAt(Position(0, 3))->kind == PieceKind::BISHOP);
    assert(board.getPieceAt(Position(0, 4))->kind == PieceKind::KNIGHT);
    assert(board.getPieceAt(Position(0, 5))->kind == PieceKind::PAWN);

    log_test("test_parse_all_piece_kinds");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING BOARD PARSER UNIT TESTS      " << std::endl;
    std::cout << "========================================" << std::endl;

    test_parse_simple_board();
    test_parse_empty_input_throws();
    test_parse_unknown_token_length_throws();
    test_parse_unknown_color_throws();
    test_parse_unknown_piece_type_throws();
    test_parse_all_piece_kinds();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL BOARD PARSER TESTS EXECUTED       " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
