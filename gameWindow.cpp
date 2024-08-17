#include "gameWindow.hpp"
#include "chessboard.hpp"
#include <iostream>

GameWindow::GameWindow(const char* title, int width, int height, Chessboard* chessboard)
    : open(true), window(nullptr), renderer(nullptr), chessboard(chessboard) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        open = false;
        return;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        open = false;
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        open = false;
        return;
    }
}

GameWindow::~GameWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Renderer* GameWindow::getRenderer() const {
    return renderer;
}

bool GameWindow::isOpen() const {
    return open;
}

// void GameWindow::handleEvents() {
//     SDL_Event event;
//     while (SDL_PollEvent(&event)) {
//         if (event.type == SDL_QUIT) {
//             open = false;
//         }
//         chessboard->handleEvent(event);
//     }   
// }
