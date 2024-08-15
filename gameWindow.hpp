#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <SDL2/SDL.h>
#include "chessboard.hpp"

class GameWindow {
public:
    GameWindow(const char* title, int width, int height, Chessboard* chessboard);
    ~GameWindow();
    SDL_Renderer* getRenderer() const;
    bool isOpen() const;
    void handleEvents();
    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool open;

    Chessboard* chessboard;
};

#endif // GAMEWINDOW_HPP