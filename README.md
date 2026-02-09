Tic-Tac-Toe Game
This logger implementation was developed on the macOS platform. The entire project is built upon the starter code and materials provided by UCSC CMPM 123 course.

Development Approach and Problem Clarification
This project implements a simple two-player XO (Tic-Tac-Toe) style game with a clear focus on interaction flow, game state management, and user feedback.

Before implementation, the core problems were clarified as follows:

How to determine game end states (win or draw) reliably.
How to lock player input once a match is finished.
How to provide clear visual and UI feedback for wins, draws, and retries.
How to ensure consistent behavior when restarting or switching game modes.
Core Features
Two-player game with Player X and Player O
Automatic turn switching between X and O
Win detection with:
Visual line connecting the three winning positions
Winner popup indicating the result
Draw detection and display in the same winner popup
Input is disabled once a match ends
Automatic retry when:
Clicking OK in the winner popup
Switching game modes
Retry button that is always visible and functional in Settings
Current player display in Settings using X / O notation
Main Classes
GameManager

Controls overall game flow
Tracks current player (X / O)
Handles game state transitions (playing, win, draw)
Triggers retry logic
Board / Grid

Stores the board state
Validates player moves
Checks win and draw conditions
Cell / Tile

Represents a single position on the board
Handles player input for that position
Renders X or O when occupied
UIManager

Displays current player information
Manages the winner popup
Handles retry and mode switching UI
Building
Build using CMake:

cd build
cmake ..
make
Run the compiled executable:

./demo
No external dependencies are required beyond the base environment.

Project Structure
.
├── classes/
│   ├── GameManager
│   ├── Board / TicTacToe
│   ├── Cell / Square
│   └── UIManager
├── imgui/
│   └── ImGui library files
├── resources/
│   └── assets and resources
├── build/
│   └── compiled output
├── Application.cpp
├── Application.h
├── CMakeLists.txt
└── README.md
classes/ contains all gameplay and logic code
imgui/ includes the ImGui UI framework
resources/ stores visual and UI resources
build/ contains compiled output
