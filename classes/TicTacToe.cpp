#include "TicTacToe.h"

namespace {
    const int kHumanMark = 1; // player 0 (O) -> state char '1'
    const int kAIMark = 2;    // player 1 (X) -> state char '2'

    int winnerForBoard(const int board[9]) {
        const int win[8][3] = {
            {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
            {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
            {0, 4, 8}, {2, 4, 6}
        };
        for (int i = 0; i < 8; ++i) {
            int a = board[win[i][0]];
            if (a != 0 && a == board[win[i][1]] && a == board[win[i][2]]) {
                return a;
            }
        }
        return 0;
    }

    bool boardHasEmpty(const int board[9]) {
        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) return true;
        }
        return false;
    }

    int negamax(int board[9], int currentPlayer) {
        int winner = winnerForBoard(board);
        if (winner != 0) {
            return (winner == currentPlayer) ? 1 : -1;
        }
        if (!boardHasEmpty(board)) return 0;

        int bestScore = -2;
        int nextPlayer = (currentPlayer == kAIMark) ? kHumanMark : kAIMark;
        for (int i = 0; i < 9; ++i) {
            if (board[i] != 0) continue;
            board[i] = currentPlayer;
            int score = -negamax(board, nextPlayer);
            board[i] = 0;
            if (score > bestScore) {
                bestScore = score;
                if (bestScore == 1) break;
            }
        }
        return bestScore;
    }

    int negamaxAlphaBeta(int board[9], int currentPlayer, int alpha, int beta) {
        int winner = winnerForBoard(board);
        if (winner != 0) {
            return (winner == currentPlayer) ? 1 : -1;
        }
        if (!boardHasEmpty(board)) return 0;

        int nextPlayer = (currentPlayer == kAIMark) ? kHumanMark : kAIMark;
        for (int i = 0; i < 9; ++i) {
            if (board[i] != 0) continue;
            board[i] = currentPlayer;
            int score = -negamaxAlphaBeta(board, nextPlayer, -beta, -alpha);
            board[i] = 0;
            if (score > alpha) alpha = score;
            if (alpha >= beta) break; // prune
        }
        return alpha;
    }
}

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = 1;      // index of the AI player (O)
const int HUMAN_PLAYER= 0;      // index of the human player (X)

TicTacToe::TicTacToe()
{
    _actionsDisabled = false;
    _hasWinner = false;
    _winningTriple[0] = _winningTriple[1] = _winningTriple[2] = -1;
    _requestRestart = false;
}

TicTacToe::~TicTacToe()
{
}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard()
{
    // Set up 2 players for the game
    setNumberOfPlayers(2);
    // Name players: index 0 -> Player O, index 1 -> Player X
    if (_players.size() >= 2) {
        _players[0]->setName("Player O");
        _players[1]->setName("Player X");
    }
    
    // Configure the game grid dimensions (3x3)
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;
    
    // Initialize the 3x3 grid of squares
    // Each square is positioned based on its grid coordinates
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            // Calculate the position: spacing squares approximately 100 pixels apart
            ImVec2 position(100.0f + x * 100.0f, 100.0f + y * 100.0f);
            
            // Initialize the square with its sprite and position
            _grid[y][x].initHolder(position, "square.png", x, y);
        }
    }
    
    // Start the game
    startGame();
    _actionsDisabled = false;
    _hasWinner = false;
    _requestRestart = false;
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool TicTacToe::actionForEmptyHolder(BitHolder *holder)
{
    // ignore if actions disabled (after win/draw)
    if (_actionsDisabled) return false;

    // 1) Guard clause: if holder is nullptr, fail fast.
    if (!holder) return false;

    // 2) Is it actually empty?
    if (!holder->empty()) return false;

    // 3) Place the current player's piece on this holder:
    //    - Get the current player
    Player *currentPlayer = getCurrentPlayer();
    if (!currentPlayer) return false;
    
    //    - Create a Bit via PieceForPlayer(currentPlayerIndex).
    Bit *newBit = PieceForPlayer(currentPlayer->playerNumber());
    if (!newBit) return false;
    
    //    - Position it at the holder's position
    newBit->setPosition(holder->getPosition());
    
    //    - Assign it to the holder
    holder->setBit(newBit);

    // 4) Return true to indicate successful placement
    return true;
}

bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
{
    // Loop through the 3x3 grid and destroy any bits on each square
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            // destroyBit handles the cleanup of the bit if it exists
            _grid[y][x].destroyBit();
        }
    }
    // reset flags
    _actionsDisabled = false;
    _hasWinner = false;
    _winningTriple[0] = _winningTriple[1] = _winningTriple[2] = -1;
    _requestRestart = false;
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index ) const
{
    // Convert linear index (0..8) to x,y coordinates
    // y = index / 3 (row)
    // x = index % 3 (column)
    int y = index / 3;
    int x = index % 3;
    
    // Check bounds
    if (x < 0 || x >= 3 || y < 0 || y >= 3) return nullptr;
    
    // Get the square at this location
    Bit *bit = _grid[y][x].bit();
    
    // If there is no bit at that location, return nullptr
    if (!bit) return nullptr;
    
    // Return the owner of the bit
    return bit->getOwner();
}

Player* TicTacToe::checkForWinner()
{
    // Define all winning combinations (8 total)
    // Using indices 0-8 representing the 3x3 grid:
    // 0 1 2
    // 3 4 5
    // 6 7 8
    const int winningCombinations[8][3] = {
        {0, 1, 2}, // top row
        {3, 4, 5}, // middle row
        {6, 7, 8}, // bottom row
        {0, 3, 6}, // left column
        {1, 4, 7}, // middle column
        {2, 5, 8}, // right column
        {0, 4, 8}, // diagonal (top-left to bottom-right)
        {2, 4, 6}  // diagonal (top-right to bottom-left)
    };
    
    // Check each winning combination
    for (int i = 0; i < 8; i++)
    {
        // Get the owner of each position in this combination
        Player *owner1 = ownerAt(winningCombinations[i][0]);
        Player *owner2 = ownerAt(winningCombinations[i][1]);
        Player *owner3 = ownerAt(winningCombinations[i][2]);

        // If all three positions have the same non-null owner, we have a winner
        if (owner1 != nullptr && owner1 == owner2 && owner2 == owner3)
        {
            // record winning triple and disable further actions
            _hasWinner = true;
            _winningTriple[0] = winningCombinations[i][0];
            _winningTriple[1] = winningCombinations[i][1];
            _winningTriple[2] = winningCombinations[i][2];
            _actionsDisabled = true;

            // compute endpoints for overlay line (center of first and third holders)
            int idxA = _winningTriple[0];
            int idxB = _winningTriple[2];
            int yA = idxA / 3; int xA = idxA % 3;
            int yB = idxB / 3; int xB = idxB % 3;
            ImVec2 posA = _grid[yA][xA].getPosition();
            ImVec2 posB = _grid[yB][xB].getPosition();
            ImVec2 sizeA = _grid[yA][xA].getSize();
            ImVec2 sizeB = _grid[yB][xB].getSize();
            _winP1 = ImVec2(posA.x + sizeA.x * 0.5f, posA.y + sizeA.y * 0.5f);
            _winP2 = ImVec2(posB.x + sizeB.x * 0.5f, posB.y + sizeB.y * 0.5f);

            return owner1;
        }
    }

    // No winner found
    return nullptr;
}

bool TicTacToe::checkForDraw()
{
    // Check if the board is full (all 9 squares have pieces)
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            // If any square is empty, the game is not a draw yet
            if (_grid[y][x].empty())
            {
                return false;
            }
        }
    }
    
    // All squares are filled and there's no winner, so it's a draw
    _actionsDisabled = true;
    return true;
}

//
// state strings
//
std::string TicTacToe::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const
{
    // Build a string representing the current state of the board
    // the string should be 9 characters long, one for each square
    // each character should be '0' for empty, '1' for player 1 (X), and '2' for player 2 (O)
    std::string state = "";
    
    // Iterate through the board left-to-right, top-to-bottom
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            Bit *bit = _grid[y][x].bit();
            
            if (bit == nullptr)
            {
                // Empty square
                state += '0';
            }
            else
            {
                // Get the owner and their player number
                Player *owner = bit->getOwner();
                if (owner)
                {
                    // Player numbers are 0-based, so add 1 to get '1' or '2'
                    state += std::to_string(owner->playerNumber() + 1);
                }
                else
                {
                    state += '0';
                }
            }
        }
    }
    
    return state;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s)
{
    // Set the state of the board from the given string
    // the string will be 9 characters long, one for each square
    // each character will be '0' for empty, '1' for player 1 (X), and '2' for player 2 (O)
    
    int index = 0;
    
    // Loop through the 3x3 array and set each square accordingly
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            if (index >= s.length()) break;
            
            // Convert the character to a player number ('0', '1', or '2')
            int playerNumber = s[index] - '0';
            
            // Clear the current square
            _grid[y][x].destroyBit();
            
            // Set the square based on the player number
            if (playerNumber == 0)
            {
                // Empty square, already cleared
            }
            else if (playerNumber == 1 || playerNumber == 2)
            {
                // Create a piece for this player (convert back to 0-based index)
                Bit *bit = PieceForPlayer(playerNumber - 1);
                if (bit)
                {
                    // Position the piece at the holder's position
                    bit->setPosition(_grid[y][x].getPosition());
                    // Set the piece in the holder
                    _grid[y][x].setBit(bit);
                }
            }
            
            index++;
        }
    }
}

void TicTacToe::renderOverlay()
{
    bool isDraw = false;
    if (!_hasWinner) {
        isDraw = checkForDraw();
    }

    if (_hasWinner) {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImU32 col = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        draw_list->AddLine(_winP1, _winP2, col, 6.0f);
    }

    if (_hasWinner || isDraw) {
        if (!ImGui::IsPopupOpen("WinnerPopup")) ImGui::OpenPopup("WinnerPopup");
        if (ImGui::BeginPopupModal("WinnerPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (isDraw) {
                ImGui::Text("Draw!");
            } else {
                Player *owner = ownerAt(_winningTriple[0]);
                std::string winnerName = "Winner";
                if (owner) {
                    int pn = owner->playerNumber();
                    winnerName = (pn == 0) ? "Player O" : "Player X";
                }
                ImGui::Text("%s wins!", winnerName.c_str());
            }
            if (ImGui::Button("OK")) {
                _requestRestart = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

bool TicTacToe::takeRestartRequest()
{
    if (_requestRestart) {
        _requestRestart = false;
        return true;
    }
    return false;
}


//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI() 
{
    // Find all empty holders
    std::vector<int> emptyIndices;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (_grid[y][x].empty()) {
                emptyIndices.push_back(y * 3 + x);
            }
        }
    }

    if (emptyIndices.empty()) return;

    // Difficulty handling: medium uses negamax; hard uses alpha-beta pruning.
    switch (_gameOptions.AIDifficulty) {
        case 0: // two-player mode; AI shouldn't run
            return;
        case 1: // easy
        default: {
            int randomIndex = rand() % emptyIndices.size();
            int idx = emptyIndices[randomIndex];
            actionForEmptyHolder(&_grid[idx / 3][idx % 3]);
            return;
        }
        case 2: { // medium: negamax
            int board[9] = {0};
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) {
                    Bit *bit = _grid[y][x].bit();
                    if (!bit) continue;
                    Player *owner = bit->getOwner();
                    if (!owner) continue;
                    int idx = y * 3 + x;
                    int pn = owner->playerNumber();
                    board[idx] = (pn == 0) ? kHumanMark : kAIMark;
                }
            }

            int bestScore = -2;
            int bestIndex = -1;
            for (int idx : emptyIndices) {
                board[idx] = kAIMark;
                int score = -negamax(board, kHumanMark);
                board[idx] = 0;
                if (score > bestScore) {
                    bestScore = score;
                    bestIndex = idx;
                    if (bestScore == 1) break;
                }
            }

            if (bestIndex >= 0) {
                actionForEmptyHolder(&_grid[bestIndex / 3][bestIndex % 3]);
            } else {
                int randomIndex = rand() % emptyIndices.size();
                int idx = emptyIndices[randomIndex];
                actionForEmptyHolder(&_grid[idx / 3][idx % 3]);
            }
            return;
        }
        case 3: { // hard: negamax with alpha-beta pruning
            int board[9] = {0};
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) {
                    Bit *bit = _grid[y][x].bit();
                    if (!bit) continue;
                    Player *owner = bit->getOwner();
                    if (!owner) continue;
                    int idx = y * 3 + x;
                    int pn = owner->playerNumber();
                    board[idx] = (pn == 0) ? kHumanMark : kAIMark;
                }
            }

            int bestScore = -2;
            int bestIndex = -1;
            for (int idx : emptyIndices) {
                board[idx] = kAIMark;
                int score = -negamaxAlphaBeta(board, kHumanMark, -2, 2);
                board[idx] = 0;
                if (score > bestScore) {
                    bestScore = score;
                    bestIndex = idx;
                    if (bestScore == 1) break;
                }
            }

            if (bestIndex >= 0) {
                actionForEmptyHolder(&_grid[bestIndex / 3][bestIndex % 3]);
            } else {
                int randomIndex = rand() % emptyIndices.size();
                int idx = emptyIndices[randomIndex];
                actionForEmptyHolder(&_grid[idx / 3][idx % 3]);
            }
            return;
        }
    }
}
