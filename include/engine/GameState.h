#pragma once

class GameState {
public:
    GameState() = default;
    explicit GameState(bool gameOver) : is_game_over(gameOver) {}

    bool isGameOver() const { return is_game_over; }
    void setGameOver(bool gameOver) { is_game_over = gameOver; }

    void markGameOver() { is_game_over = true; }
    void reset() { is_game_over = false; }

private:
    bool is_game_over = false;
};
