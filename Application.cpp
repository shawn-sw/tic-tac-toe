#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"

namespace ClassGame {
        //
        // our global variables
        //
        TicTacToe *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = new TicTacToe();
            game->_gameOptions.AIPlaying = false;  // Start with two-player mode
            game->setUpBoard();
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                if (!game) return;
                if (!game->getCurrentPlayer()) return;
                
                ImGui::Begin("Settings");
                
                // Game mode selection buttons
                ImGui::Text("Game Mode:");
                if (ImGui::Button("Two Player", ImVec2(120, 0))) {
                    game->_gameOptions.AIPlaying = false;
                    game->stopGame();
                    game->setUpBoard();
                    gameOver = false;
                    gameWinner = -1;
                }
                ImGui::SameLine();
                if (ImGui::Button("vs AI", ImVec2(120, 0))) {
                    game->_gameOptions.AIPlaying = true;
                    game->setAIPlayer(1);  // AI is player 1 (X)
                    game->stopGame();
                    game->setUpBoard();
                    gameOver = false;
                    gameWinner = -1;
                }
                
                ImGui::Separator();
                
                // Show current player as O or X
                if (game->getCurrentPlayer()) {
                    int pn = game->getCurrentPlayer()->playerNumber();
                    const char *symbol = (pn == 0) ? "O" : "X";
                    ImGui::Text("Current Player: %s", symbol);
                    if (game->_gameOptions.AIPlaying && pn == 1) {
                        ImGui::SameLine();
                        ImGui::Text("(AI)");
                    }
                } else {
                    ImGui::Text("Current Player: -");
                }
                ImGui::Text("Current Board State: %s", game->stateString().c_str());

                // Always-visible Retry button
                if (ImGui::Button("Retry")) {
                    game->stopGame();
                    game->setUpBoard();
                    gameOver = false;
                    gameWinner = -1;
                }
                if (gameOver) {
                    ImGui::Text("Game Over!");
                    if (gameWinner >= 0) {
                        const char *symbol = (gameWinner == 0) ? "O" : "X";
                        ImGui::Text("Winner: %s", symbol);
                    } else {
                        ImGui::Text("Winner: Draw");
                    }
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                game->drawFrame();
                // If TicTacToe, render overlay (popups/lines) and handle restart
                TicTacToe *ttt = dynamic_cast<TicTacToe*>(game);
                if (ttt) {
                    ttt->renderOverlay();
                    if (ttt->takeRestartRequest()) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                    }
                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
            
            // If AI is enabled and it's the AI's turn, make the AI move
            if (!gameOver && game->_gameOptions.AIPlaying && game->getCurrentPlayer()) {
                if (game->getCurrentPlayer()->playerNumber() == 1) {  // AI is player 1
                    game->updateAI();
                    game->endTurn();  // Continue to next player
                    
                    // Check for winner/draw after AI move
                    winner = game->checkForWinner();
                    if (winner)
                    {
                        gameOver = true;
                        gameWinner = winner->playerNumber();
                    }
                    if (game->checkForDraw()) {
                        gameOver = true;
                        gameWinner = -1;
                    }
                }
            }
        }
}
