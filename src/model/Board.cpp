#pragma once
#include "../../include/model/Board.h"
#include <stdexcept>

Board::Board(int width, int height) : width(width), height(height) {}

bool Board::isInsideBounds(const Position& pos) const {
    return pos.row >= 0 && pos.row < height && pos.col >= 0 && pos.col < width;
}

bool Board::addPiece(const Position& pos, std::unique_ptr<Piece> piece) {
    if (!isInsideBounds(pos)) {
        return false;
    }
    if (pieces.find(pos) != pieces.end()) {
        return false;
    }
    pieces[pos] = std::move(piece);
    return true;
}

bool Board::removePiece(const Position& pos) {
    auto it = pieces.find(pos);
    if (it != pieces.end()) {
        pieces.erase(it);
        return true;
    }
    return false;
}

Piece* Board::replacePiece(const Position& pos, std::unique_ptr<Piece> piece) {
    if (!isInsideBounds(pos)) {
        return nullptr;
    }
    Piece* raw = piece.get();
    pieces[pos] = std::move(piece);
    return raw;
}

Piece* Board::getPieceAt(const Position& pos) const {
    auto it = pieces.find(pos);
    return (it != pieces.end()) ? it->second.get() : nullptr;
}

void Board::movePiece(const Position& source, const Position& destination) {
    auto it = pieces.find(source);
    if (it != pieces.end()) {
        pieces[destination] = std::move(it->second);
        pieces[destination]->setPosition(destination);
        pieces.erase(it);
    }
}

std::vector<Piece*> Board::getAllPieces() const {
    std::vector<Piece*> result;
    for (auto const& [pos, piece] : pieces) {
        result.push_back(piece.get());
    }
    return result;
}