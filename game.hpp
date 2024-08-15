#ifndef GAME_HPP
#define GAME_HPP

#include "gameWindow.hpp"
#include "chessboard.hpp"

class Game {
public:
    Game();
    void run();
    
private:
    GameWindow window;
    Chessboard chessboard;
};

#endif // GAME_H
