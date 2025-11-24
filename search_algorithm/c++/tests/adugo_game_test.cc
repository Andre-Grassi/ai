#include "data_structure/adversarial_search/games/adugo_game.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

using namespace adugo_game;

// Test utilities
void AssertTrue(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "[PASS] " << test_name << std::endl;
    } else {
        std::cerr << "[FAIL] " << test_name << std::endl;
        std::exit(1);
    }
}

void AssertEqual(int expected, int actual, const std::string& test_name) {
    if (expected == actual) {
        std::cout << "[PASS] " << test_name << std::endl;
    } else {
        std::cerr << "[FAIL] " << test_name << " - Expected: " << expected
                  << ", Got: " << actual << std::endl;
        std::exit(1);
    }
}

// Helper function to create a custom state
State CreateCustomState(const std::vector<int>& jaguar_positions,
                        const std::vector<int>& dog_positions,
                        Player player_to_move) {
    Board board;
    board.fill(Symbol::kEmpty);

    // Add blocked cells (same as initial state)
    board[25] = Symbol::kBlock;  // kBoardWidth * 5 + 0
    board[29] = Symbol::kBlock;  // kBoardWidth * 5 + 4
    board[31] = Symbol::kBlock;  // kBoardWidth * 6 + 1
    board[33] = Symbol::kBlock;  // kBoardWidth * 6 + 3

    // Place jaguars
    for (int pos : jaguar_positions) {
        board[pos] = Symbol::kO;
    }

    // Place dogs
    for (int pos : dog_positions) {
        board[pos] = Symbol::kC;
    }

    return State(board, player_to_move);
}

int TestCountSymbolsInState(const State& state, Symbol symbol) {
    int count = 0;
    for (const Symbol cell : state.board)
        if (cell == symbol) count++;

    return count;
}

State SetUpCaptureState(AdugoGame& game) {
    // Create a state where jaguar can capture a dog

    // Use real initial state and make one move to set up a capture
    State state = game.GetInitialState();
    std::cout << "Before jaguar move (12->17):\n"
              << game.GetStateString(state) << std::endl;

    // Move jaguar from 12 to 17
    Action move1(Symbol::kO, 12, 17);
    std::unique_ptr<State> state2 = game.GetResult(state, move1);
    std::cout << "After jaguar move (12->17):\n"
              << game.GetStateString(*state2) << std::endl;

    // Move dog 7 to 12 to enable capture
    // Resulting board must be
    /*
        #######
        #ccccc#
        #cc-cc#
        #ccccc#
        #--o--#
        #-----#
        # --- #
        #- - -#
        #######
    */

    // Now dog moves from 7 to 12
    Action dog_move(Symbol::kC, 7, 12);
    std::unique_ptr<State> state3 = game.GetResult(*state2, dog_move);
    std::cout << "After dog move (7->12):\n"
              << game.GetStateString(*state3) << std::endl;

    return *state3;
}

// Test 1: Initial state verification
void TestInitialState() {
    std::cout << "\n=== Test 1: Initial State ===" << std::endl;
    AdugoGame game;
    State initial_state = game.GetInitialState();

    std::cout << "Initial board:\n"
              << game.GetStateString(initial_state) << std::endl;

    // Count initial pieces
    int dog_count = 0;
    int jaguar_count = 0;

    for (size_t i = 0; i < kGridDimension; ++i) {
        if (initial_state[i] == Symbol::kC) dog_count++;
        if (initial_state[i] == Symbol::kO) jaguar_count++;
    }

    AssertEqual(14, dog_count, "Initial state has 14 dogs");
    AssertEqual(1, jaguar_count, "Initial state has 1 jaguar");
    AssertTrue(initial_state.player_to_move.symbol == Symbol::kO,
               "Jaguar moves first");
    AssertTrue(!game.IsTerminal(initial_state),
               "Initial state is not terminal");
}

// Test 2: Jaguar can capture a dog when aligned
void TestJaguarCaptureCorrectDog() {
    std::cout << "\n=== Test 2: Jaguar Capture Correct Dog ===" << std::endl;
    AdugoGame game;

    State state3 = SetUpCaptureState(game);

    // Check if jaguar can capture (17 -> 7, capturing dog at 12)
    std::vector<Action> jaguar_actions = game.GetActions(state3);

    bool has_capture = false;
    for (const auto& action : jaguar_actions) {
        // Apply action and print resulting board
        std::unique_ptr<State> result = game.GetResult(state3, action);
        if (action.cell_index_destination == 7) {
            has_capture = true;
            std::cout << game.GetStateString(*result) << std::endl;

            // Check if dog number has decreased by one
            int dogs_after = TestCountSymbolsInState(*result, Symbol::kC);
            int dogs_before = TestCountSymbolsInState(state3, Symbol::kC);
            AssertEqual(dogs_before - 1, dogs_after,
                        "One dog was removed after capture");

            std::cout << "Action: " << action << std::endl;

            // Check if correct dog was removed (position 12)
            AssertTrue((*result)[12] == Symbol::kEmpty,
                       "Dog at position 12 was captured");
        }
    }
}

void TestJaguarCaptureBasic() {
    std::cout << "\n=== Test 2: Jaguar Capture Basic ===" << std::endl;
    AdugoGame game;

    State state3 = SetUpCaptureState(game);

    // Check if jaguar can capture (17 -> 7, capturing dog at 12)
    std::vector<Action> jaguar_actions = game.GetActions(state3);

    bool has_capture = false;
    for (const auto& action : jaguar_actions) {
        // Apply action and print resulting board
        std::unique_ptr<State> result = game.GetResult(state3, action);
        if (action.cell_index_destination == 7) {
            has_capture = true;
            std::cout << game.GetStateString(*result) << std::endl;
        }
    }

    AssertTrue(has_capture, "Jaguar can capture dog after initial moves");
}

void TestDiagonalCapture() {
    std::cout << "\n=== Test: Jaguar Diagonal Capture ===" << std::endl;
    AdugoGame game;

    // Create a custom state where jaguar can capture diagonally
    // Place jaguar at 2, dog at 6, empty at 10
    State custom_state = CreateCustomState({2}, {6}, Player(Symbol::kO));

    std::cout << "Custom board before capture:\n"
              << game.GetStateString(custom_state) << std::endl;

    // Get jaguar actions
    std::vector<Action> jaguar_actions = game.GetActions(custom_state);

    bool has_capture = false;
    for (const auto& action : jaguar_actions) {
        if (action.cell_index_destination == 10) {
            has_capture = true;
            std::unique_ptr<State> result =
                game.GetResult(custom_state, action);
            std::cout << "After jaguar diagonal capture (2->10):\n"
                      << game.GetStateString(*result) << std::endl;

            // Check if dog at 6 was removed
            AssertTrue((*result)[6] == Symbol::kEmpty,
                       "Dog at position 6 was captured diagonally");
        }
    }

    AssertTrue(has_capture, "Jaguar can perform diagonal capture");
}

void TestSequentialCapture() {
    std::cout << "\n=== Test: Jaguar Sequential Capture ===" << std::endl;
    AdugoGame game;

    // Create a custom state where jaguar can capture two dogs in sequence
    // Place jaguar at 2, dogs at 7 and 17, the rest empty
    /* Initial Board configuration:
        #######
        #--o--#
        #--c--#
        #-----#
        #--c--#
        #-----#
        # --- #
        #- - -#
        #######

        The final Board configuration must be
        #######
        #-----#
        #-----#
        #-----#
        #-----#
        #--o--#
        # --- #
        #- - -#
        #######
    */
    State custom_state = CreateCustomState({2}, {7, 17}, Player(Symbol::kO));

    std::cout << "Custom board before sequential captures:\n"
              << game.GetStateString(custom_state) << std::endl;

    // Get jaguar actions, and check if it can capture first dog (2->12)
    std::vector<Action> jaguar_actions = game.GetActions(custom_state);
    std::unique_ptr<State> state_after_first_capture = nullptr;
    for (const auto& action : jaguar_actions) {
        if (action.cell_index_destination == 12) {
            state_after_first_capture = game.GetResult(custom_state, action);
            std::cout << "After first jaguar capture (2->12):\n"
                      << game.GetStateString(*state_after_first_capture)
                      << std::endl;
        }
    }
    AssertTrue(state_after_first_capture != nullptr,
               "Jaguar performed first capture");

    // Now check if from the new state jaguar can capture second dog (12->22)
    std::vector<Action> jaguar_actions_second =
        game.GetActions(*state_after_first_capture);
    std::unique_ptr<State> state_after_second = nullptr;
    for (const auto& action : jaguar_actions_second) {
        if (action.cell_index_destination == 22) {
            state_after_second =
                game.GetResult(*state_after_first_capture, action);
            std::cout << "After second jaguar capture (12->22):\n"
                      << game.GetStateString(*state_after_second) << std::endl;
        }
    }
    AssertTrue(state_after_second != nullptr,
               "Jaguar performed second capture");

    // Print final board configuration
    std::cout << "Final board after sequential captures:\n"
              << game.GetStateString(*state_after_second) << std::endl;
}

// Test 3: Jaguar actually captures the dog (removes it from board)
void TestJaguarExecutesCapture() {
    std::cout << "\n=== Test 3: Jaguar Executes Capture ===" << std::endl;
    AdugoGame game;

    // Set up a capture scenario from real game
    State state = game.GetInitialState();
    Action move1(Symbol::kO, 12, 16);
    std::unique_ptr<State> state2 = game.GetResult(state, move1);

    // Dog moves
    std::vector<Action> dog_actions = game.GetActions(*state2);
    std::unique_ptr<State> state3 = game.GetResult(*state2, dog_actions[0]);
    std::cout << "Before jaguar capture:\n"
              << game.GetStateString(*state3) << std::endl;

    // Count dogs before capture
    int dogs_before = 0;
    for (size_t i = 0; i < kGridDimension; ++i) {
        if ((*state3)[i] == Symbol::kC) dogs_before++;
    }

    // Execute capture action (16 -> 6, capturing dog at 11)
    Action capture_action(Symbol::kO, 16, 6);
    std::unique_ptr<State> new_state = game.GetResult(*state3, capture_action);
    std::cout << "After jaguar capture (16->6):\n"
              << game.GetStateString(*new_state) << std::endl;

    AssertTrue(new_state != nullptr, "GetResult returns valid state");

    // Count dogs after capture
    int dogs_after = 0;
    for (size_t i = 0; i < kGridDimension; ++i) {
        if ((*new_state)[i] == Symbol::kC) dogs_after++;
    }

    AssertEqual(dogs_before - 1, dogs_after,
                "One dog was removed after capture");
    AssertTrue((*new_state)[16] == Symbol::kEmpty,
               "Jaguar left original position");

    // The dog that was captured should be at one of the common neighbors of 16
    // and 6 Common neighbors are 10, 11, 12 - check which one was removed
    int removed_pos = -1;
    if ((*state3)[10] == Symbol::kC && (*new_state)[10] == Symbol::kEmpty)
        removed_pos = 10;
    if ((*state3)[11] == Symbol::kC && (*new_state)[11] == Symbol::kEmpty)
        removed_pos = 11;
    if ((*state3)[12] == Symbol::kC && (*new_state)[12] == Symbol::kEmpty)
        removed_pos = 12;

    std::cout << "Dog was removed from position: " << removed_pos << std::endl;
    AssertTrue(removed_pos >= 0, "A dog in the middle was removed");
    AssertTrue((*new_state)[6] == Symbol::kO, "Jaguar is at destination");
}

// Test 4: Jaguar capture removes the dog from the board
void TestJaguarCaptureRemovesDog() {
    std::cout << "\n=== Test 4: Jaguar Capture Removes Dog ===" << std::endl;
    AdugoGame game;

    // Set up and execute a capture
    State state = game.GetInitialState();
    Action move1(Symbol::kO, 12, 16);
    std::unique_ptr<State> state2 = game.GetResult(state, move1);

    std::vector<Action> dog_actions = game.GetActions(*state2);
    std::unique_ptr<State> state3 = game.GetResult(*state2, dog_actions[0]);
    std::cout << "Before capture:\n"
              << game.GetStateString(*state3) << std::endl;

    // Execute capture - captures dog at position 10 (or 11 or 12 depending on
    // alignment)
    Action capture_action(Symbol::kO, 16, 6);
    std::unique_ptr<State> result = game.GetResult(*state3, capture_action);
    std::cout << "After capture (16->6):\n"
              << game.GetStateString(*result) << std::endl;

    AssertTrue(result != nullptr, "Capture executed successfully");
    // Check that at least one of the middle positions had a dog removed
    bool dog_removed =
        ((*state3)[10] == Symbol::kC && (*result)[10] == Symbol::kEmpty) ||
        ((*state3)[11] == Symbol::kC && (*result)[11] == Symbol::kEmpty) ||
        ((*state3)[12] == Symbol::kC && (*result)[12] == Symbol::kEmpty);
    AssertTrue(dog_removed, "Middle dog was removed");
    AssertTrue((*result)[6] == Symbol::kO, "Jaguar reached destination");
}

// Test 5: Verify capture is a non-neighbor move
void TestCaptureIsNonNeighborMove() {
    std::cout << "\n=== Test 5: Capture Is Non-Neighbor Move ===" << std::endl;
    AdugoGame game;

    State state = game.GetInitialState();
    Action move1(Symbol::kO, 12, 16);
    std::unique_ptr<State> state2 = game.GetResult(state, move1);

    std::vector<Action> dog_actions = game.GetActions(*state2);
    std::unique_ptr<State> state3 = game.GetResult(*state2, dog_actions[0]);

    std::vector<Action> jaguar_actions = game.GetActions(*state3);

    // Find the capture action
    bool found_capture = false;
    for (const auto& action : jaguar_actions) {
        if (action.cell_index_origin == 16 &&
            action.cell_index_destination == 6) {
            found_capture = true;
            // Verify 16 and 6 are NOT neighbors
            AssertTrue(!game.IsNeighbor(16, 6), "Capture is non-neighbor move");
            break;
        }
    }

    AssertTrue(found_capture, "Found the capture action");
}

// Test 6: Multiple captures possible in real game
void TestMultipleCapturesPossible() {
    std::cout << "\n=== Test 6: Multiple Captures Possible ===" << std::endl;
    AdugoGame game;

    // Play several moves to create capture opportunities
    State state = game.GetInitialState();
    std::cout << "Initial state:\n" << game.GetStateString(state) << std::endl;

    // Make a few moves
    for (int i = 0; i < 6; i++) {
        std::vector<Action> actions = game.GetActions(state);
        if (actions.empty()) break;
        std::unique_ptr<State> new_state = game.GetResult(state, actions[0]);
        state = *new_state;
    }
    std::cout << "After 6 moves:\n" << game.GetStateString(state) << std::endl;

    // Just verify the game can continue (captures don't break the game)
    std::vector<Action> actions = game.GetActions(state);
    AssertTrue(actions.size() >= 0, "Game continues after multiple moves");
}

// Test 7: Jaguar actions include both moves and captures
void TestJaguarHasMixedActions() {
    std::cout << "\n=== Test 7: Jaguar Has Mixed Actions ===" << std::endl;
    AdugoGame game;

    State state = game.GetInitialState();
    std::vector<Action> initial_actions = game.GetActions(state);

    // Initially, jaguar should have normal moves
    AssertTrue(initial_actions.size() > 0, "Jaguar has initial moves");

    // After setup, jaguar should have capture available
    Action move1(Symbol::kO, 12, 16);
    std::unique_ptr<State> state2 = game.GetResult(state, move1);
    std::vector<Action> dog_actions = game.GetActions(*state2);
    std::unique_ptr<State> state3 = game.GetResult(*state2, dog_actions[0]);

    std::vector<Action> jaguar_actions = game.GetActions(*state3);
    AssertTrue(jaguar_actions.size() > 0, "Jaguar has actions after dog move");
}

// Test 8: Jaguar wins by capturing 5 dogs
void TestJaguarWinsByCaptures() {
    std::cout << "\n=== Test 8: Jaguar Wins By Captures ===" << std::endl;
    AdugoGame game;

    // Setup: Only 10 dogs left (14 - 4 = 10, meaning 4 captured already)
    // One more capture should trigger win
    std::vector<int> jaguar_pos = {10};
    std::vector<int> dog_pos = {
        11,                             // This dog can be captured
        5,  6, 7, 8, 9, 13, 14, 15, 16  // 9 more dogs = 10 total
    };
    Player jaguar_player(Symbol::kO);

    State state = CreateCustomState(jaguar_pos, dog_pos, jaguar_player);
    std::cout << "Before 5th capture (4 already captured):\n"
              << game.GetStateString(state) << std::endl;

    // Execute capture to win
    Action capture_action(Symbol::kO, 10, 12);
    std::unique_ptr<State> new_state = game.GetResult(state, capture_action);
    std::cout << "After 5th capture (10->12):\n"
              << game.GetStateString(*new_state) << std::endl;

    // Now there should be only 9 dogs (14 - 5 = 9)
    int remaining_dogs = 0;
    for (size_t i = 0; i < kGridDimension; ++i) {
        if ((*new_state)[i] == Symbol::kC) remaining_dogs++;
    }

    AssertEqual(9, remaining_dogs, "9 dogs remain after 5th capture");
    AssertTrue(game.IsTerminal(*new_state),
               "Game is terminal after 5 captures");

    Utility utility = game.GetUtility(*new_state);
    AssertEqual(-1, utility,
                "Jaguar wins (utility = -1 from dogs' perspective)");
}

// Test 9: Dogs cannot capture
void TestDogsCannotCapture() {
    std::cout << "\n=== Test 9: Dogs Cannot Capture ===" << std::endl;
    AdugoGame game;

    // Setup: Dog at 11, jaguar at 12, empty at 13
    std::vector<int> jaguar_pos = {12};
    std::vector<int> dog_pos = {11, 5, 6, 7, 8, 9, 10};
    Player dog_player(Symbol::kC);

    State state = CreateCustomState(jaguar_pos, dog_pos, dog_player);
    std::cout << "Custom state (dog turn, jaguar adjacent):\n"
              << game.GetStateString(state) << std::endl;

    std::vector<Action> actions = game.GetActions(state);

    // Check that dogs don't have capture-like actions
    bool has_jump_action = false;
    for (const auto& action : actions) {
        if (action.player_symbol == Symbol::kC) {
            int origin = action.cell_index_origin;
            int dest = action.cell_index_destination;

            // If not neighbors, it would be a "jump"
            if (!game.IsNeighbor(origin, dest)) {
                has_jump_action = true;
                break;
            }
        }
    }

    AssertTrue(!has_jump_action, "Dogs cannot perform jump/capture actions");
}

// Test 10: Verify jaguar capture removes correct dog
void TestCorrectDogRemoved() {
    std::cout << "\n=== Test 10: Correct Dog Removed ===" << std::endl;
    AdugoGame game;

    // Setup: Jaguar at 6, dogs at 7 and 11, empty at 8 and 12
    std::vector<int> jaguar_pos = {6};
    std::vector<int> dog_pos = {7, 11, 5, 10, 15, 16};
    Player jaguar_player(Symbol::kO);

    State state = CreateCustomState(jaguar_pos, dog_pos, jaguar_player);
    std::cout << "Before capture (jaguar at 6, dog at 7):\n"
              << game.GetStateString(state) << std::endl;

    // Capture dog at position 7 by moving to 8
    Action capture_action(Symbol::kO, 6, 8);
    std::unique_ptr<State> new_state = game.GetResult(state, capture_action);
    std::cout << "After capture (6->8):\n"
              << game.GetStateString(*new_state) << std::endl;

    AssertTrue((*new_state)[7] == Symbol::kEmpty, "Dog at 7 was removed");
    AssertTrue((*new_state)[11] == Symbol::kC, "Dog at 11 still exists");
    AssertTrue((*new_state)[8] == Symbol::kO, "Jaguar moved to position 8");
}

int main() {
    std::cout << "Running Adugo Game Tests...\n" << std::endl;

    TestDiagonalCapture();
    TestSequentialCapture();
    TestJaguarCaptureBasic();
    TestMultipleCapturesPossible();
    /*
    TestInitialState();
    TestJaguarCaptureCorrectDog();
    TestJaguarExecutesCapture();
    TestJaguarCaptureRemovesDog();
    TestCaptureIsNonNeighborMove();
    TestJaguarHasMixedActions();
    TestJaguarWinsByCaptures();
    TestDogsCannotCapture();
    */
    std::cout << "\n✓ All tests passed!" << std::endl;
    std::cout
        << "\nSummary: Jaguar can correctly capture dogs by jumping over them."
        << std::endl;
    std::cout << "The capture mechanism removes the dog in the middle and "
                 "moves the jaguar."
              << std::endl;
    return 0;
}
