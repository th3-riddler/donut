#include "game.hpp"
#include "macros.hpp"
#include <iostream>
#include "preComputedMoveData.hpp"

Game::Game()
    : chessboard(window.getRenderer()), window("Chessboard", 800, 800, &chessboard) {
        // std::cout << "Game constructor: chessboard address = " << &chessboard << std::endl;
    }

void Game::run() {
    // std::cout << "Game::run: chessboard address = " << &chessboard << std::endl;
    // chessboard.initializeFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //chessboard.initializeFromFEN("8/8/8/8/3R4/8/8/8 w - - 0 1");
    preComputedMoveData::initialize();
    chessboard.getPawnMoves();
    // while (window.isOpen()) {
    //     // std::cout << "Handling events..." << std::endl;
    //     window.handleEvents();
        
    //     SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 255); // Black background
    //     SDL_RenderClear(window.getRenderer());


    //     // std::cout << "Drawing game state..." << std::endl;
    //     chessboard.drawGameState();

    //     SDL_RenderPresent(window.getRenderer());
    //     SDL_Delay(16); // Approximate 60 FPS
    // }
    // preComputedMoveData::cleanupNumSquaresToEdge();
}