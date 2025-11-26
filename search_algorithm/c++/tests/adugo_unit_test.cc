#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "data_structure/adversarial_search/games/adugo_game.h"

using namespace adugo_game;

// Test utilities
void AssertTrue(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "  [PASS] " << test_name << std::endl;
    } else {
        std::cerr << "  [FAIL] " << test_name << std::endl;
        std::exit(1);
    }
}

void AssertFalse(bool condition, const std::string& test_name) {
    AssertTrue(!condition, test_name);
}

void AssertEqual(int expected, int actual, const std::string& test_name) {
    if (expected == actual) {
        std::cout << "  [PASS] " << test_name << std::endl;
    } else {
        std::cerr << "  [FAIL] " << test_name << " - Expected: " << expected
                  << ", Got: " << actual << std::endl;
        std::exit(1);
    }
}

template <typename T>
void AssertOptionalHasValue(const std::optional<T>& opt,
                            const std::string& test_name) {
    if (opt.has_value()) {
        std::cout << "  [PASS] " << test_name << std::endl;
    } else {
        std::cerr << "  [FAIL] " << test_name
                  << " - Expected value, got nullopt" << std::endl;
        std::exit(1);
    }
}

void AssertOptionalEmpty(const std::optional<int>& opt,
                         const std::string& test_name) {
    if (!opt.has_value()) {
        std::cout << "  [PASS] " << test_name << std::endl;
    } else {
        std::cerr << "  [FAIL] " << test_name
                  << " - Expected nullopt, got: " << opt.value() << std::endl;
        std::exit(1);
    }
}

// ============================================================================
// TEST: IsNeighbor
// ============================================================================

void TestIsNeighborBasic() {
    std::cout << "\n=== Test: IsNeighbor - Basic ===" << std::endl;
    AdugoGame game;

    // Test horizontal neighbors
    AssertTrue(game.IsNeighbor(0, 1), "0 and 1 are horizontal neighbors");
    AssertTrue(game.IsNeighbor(1, 0), "1 and 0 are neighbors (symmetric)");
    AssertTrue(game.IsNeighbor(2, 3), "2 and 3 are horizontal neighbors");

    // Test vertical neighbors
    AssertTrue(game.IsNeighbor(0, 5), "0 and 5 are vertical neighbors");
    AssertTrue(game.IsNeighbor(5, 10), "5 and 10 are vertical neighbors");
    AssertTrue(game.IsNeighbor(10, 15), "10 and 15 are vertical neighbors");

    // Test diagonal neighbors
    AssertTrue(game.IsNeighbor(0, 6), "0 and 6 are diagonal neighbors");
    AssertTrue(game.IsNeighbor(6, 12), "6 and 12 are diagonal neighbors");
    AssertTrue(game.IsNeighbor(2, 8), "2 and 8 are diagonal neighbors");
}

void TestIsNeighborNonNeighbors() {
    std::cout << "\n=== Test: IsNeighbor - Non-Neighbors ===" << std::endl;
    AdugoGame game;

    // Test non-neighbors (too far apart)
    AssertFalse(game.IsNeighbor(0, 10), "0 and 10 are not neighbors");
    AssertFalse(game.IsNeighbor(0, 12), "0 and 12 are not neighbors");
    AssertFalse(game.IsNeighbor(5, 15), "5 and 15 are not neighbors");

    // Test same position
    AssertFalse(game.IsNeighbor(5, 5), "Position cannot be neighbor to itself");
    AssertFalse(game.IsNeighbor(12, 12),
                "Position cannot be neighbor to itself");
}

void TestIsNeighborTriangleArea() {
    std::cout << "\n=== Test: IsNeighbor - Triangle Area ===" << std::endl;
    AdugoGame game;

    // Test triangle area neighbors
    AssertTrue(game.IsNeighbor(22, 26), "22 and 26 are neighbors");
    AssertTrue(game.IsNeighbor(22, 27), "22 and 27 are neighbors");
    AssertTrue(game.IsNeighbor(22, 28), "22 and 28 are neighbors");
    AssertTrue(game.IsNeighbor(26, 27), "26 and 27 are neighbors");
    AssertTrue(game.IsNeighbor(26, 30), "26 and 30 are neighbors");
    AssertTrue(game.IsNeighbor(27, 32), "27 and 32 are neighbors");
    AssertTrue(game.IsNeighbor(30, 32), "30 and 32 are neighbors");
    AssertTrue(game.IsNeighbor(32, 34), "32 and 34 are neighbors");
}

void TestIsNeighborInvalidPositions() {
    std::cout << "\n=== Test: IsNeighbor - Invalid Positions ===" << std::endl;
    AdugoGame game;

    // Test blocked/invalid positions
    AssertFalse(game.IsNeighbor(25, 26), "25 is blocked, no neighbors");
    AssertFalse(game.IsNeighbor(29, 28), "29 is blocked, no neighbors");
    AssertFalse(game.IsNeighbor(31, 30), "31 is blocked, no neighbors");
    AssertFalse(game.IsNeighbor(33, 32), "33 is blocked, no neighbors");
}

// ============================================================================
// TEST: FindMiddlePosition
// ============================================================================

void TestFindMiddlePositionHorizontal() {
    std::cout << "\n=== Test: FindMiddlePosition - Horizontal ===" << std::endl;
    AdugoGame game;

    // Horizontal middle positions
    auto middle = game.FindMiddlePosition(0, 2);
    AssertOptionalHasValue(middle, "Middle exists between 0 and 2");
    AssertEqual(1, middle.value(), "Middle between 0 and 2 is 1");

    middle = game.FindMiddlePosition(5, 7);
    AssertOptionalHasValue(middle, "Middle exists between 5 and 7");
    AssertEqual(6, middle.value(), "Middle between 5 and 7 is 6");

    middle = game.FindMiddlePosition(10, 12);
    AssertOptionalHasValue(middle, "Middle exists between 10 and 12");
    AssertEqual(11, middle.value(), "Middle between 10 and 12 is 11");
}

void TestFindMiddlePositionVertical() {
    std::cout << "\n=== Test: FindMiddlePosition - Vertical ===" << std::endl;
    AdugoGame game;

    // Vertical middle positions
    auto middle = game.FindMiddlePosition(0, 10);
    AssertOptionalHasValue(middle, "Middle exists between 0 and 10");
    AssertEqual(5, middle.value(), "Middle between 0 and 10 is 5");

    middle = game.FindMiddlePosition(1, 11);
    AssertOptionalHasValue(middle, "Middle exists between 1 and 11");
    AssertEqual(6, middle.value(), "Middle between 1 and 11 is 6");

    middle = game.FindMiddlePosition(6, 16);
    AssertOptionalHasValue(middle, "Middle exists between 6 and 16");
    AssertEqual(11, middle.value(), "Middle between 6 and 16 is 11");
}

void TestFindMiddlePositionDiagonal() {
    std::cout << "\n=== Test: FindMiddlePosition - Diagonal ===" << std::endl;
    AdugoGame game;

    // Diagonal middle positions
    auto middle = game.FindMiddlePosition(0, 12);
    AssertOptionalHasValue(middle, "Middle exists between 0 and 12");
    AssertEqual(6, middle.value(), "Middle between 0 and 12 is 6");

    middle = game.FindMiddlePosition(2, 12);
    AssertOptionalHasValue(middle, "Middle exists between 2 and 12");
    AssertEqual(7, middle.value(), "Middle between 2 and 12 is 7");

    middle = game.FindMiddlePosition(10, 22);
    AssertOptionalHasValue(middle, "Middle exists between 10 and 22");
    AssertEqual(16, middle.value(), "Middle between 10 and 22 is 16");
}

void TestFindMiddlePositionNoMiddle() {
    std::cout << "\n=== Test: FindMiddlePosition - No Middle ===" << std::endl;
    AdugoGame game;

    // Adjacent positions (no middle)
    auto middle = game.FindMiddlePosition(0, 1);
    AssertOptionalEmpty(middle, "No middle between adjacent positions 0 and 1");

    middle = game.FindMiddlePosition(6, 7);
    AssertOptionalEmpty(middle, "No middle between adjacent positions 6 and 7");

    // Non-aligned positions
    middle = game.FindMiddlePosition(0, 14);
    AssertOptionalEmpty(middle, "No middle between non-aligned 0 and 14");

    middle = game.FindMiddlePosition(0, 24);
    AssertOptionalEmpty(middle, "No middle between non-aligned 0 and 24");
}

void TestFindMiddlePositionSymmetry() {
    std::cout << "\n=== Test: FindMiddlePosition - Symmetry ===" << std::endl;
    AdugoGame game;

    // Test that order doesn't matter
    auto middle1 = game.FindMiddlePosition(0, 10);
    auto middle2 = game.FindMiddlePosition(10, 0);

    AssertOptionalHasValue(middle1, "Middle exists for (0, 10)");
    AssertOptionalHasValue(middle2, "Middle exists for (10, 0)");
    AssertEqual(middle1.value(), middle2.value(),
                "Middle is same regardless of order");

    middle1 = game.FindMiddlePosition(2, 12);
    middle2 = game.FindMiddlePosition(12, 2);
    AssertEqual(middle1.value(), middle2.value(),
                "Middle is same for (2, 12) and (12, 2)");
}

// ============================================================================
// TEST: IsAligned
// ============================================================================

void TestIsAlignedHorizontal() {
    std::cout << "\n=== Test: IsAligned - Horizontal ===" << std::endl;
    AdugoGame game;

    // Test horizontal alignments
    AssertTrue(game.IsAligned(0, 1, 2), "0-1-2 are horizontally aligned");
    AssertTrue(game.IsAligned(5, 6, 7), "5-6-7 are horizontally aligned");
    AssertTrue(game.IsAligned(10, 11, 12), "10-11-12 are horizontally aligned");
    AssertTrue(game.IsAligned(15, 16, 17), "15-16-17 are horizontally aligned");

    // Test symmetry
    AssertTrue(game.IsAligned(2, 1, 0),
               "2-1-0 are horizontally aligned (reverse)");
}

void TestIsAlignedVertical() {
    std::cout << "\n=== Test: IsAligned - Vertical ===" << std::endl;
    AdugoGame game;

    // Test vertical alignments
    AssertTrue(game.IsAligned(0, 5, 10), "0-5-10 are vertically aligned");
    AssertTrue(game.IsAligned(1, 6, 11), "1-6-11 are vertically aligned");
    AssertTrue(game.IsAligned(2, 7, 12), "2-7-12 are vertically aligned");
    AssertTrue(game.IsAligned(6, 11, 16), "6-11-16 are vertically aligned");

    // Test symmetry
    AssertTrue(game.IsAligned(10, 5, 0),
               "10-5-0 are vertically aligned (reverse)");
}

void TestIsAlignedDiagonal() {
    std::cout << "\n=== Test: IsAligned - Diagonal ===" << std::endl;
    AdugoGame game;

    // Test diagonal alignments (top-left to bottom-right)
    AssertTrue(game.IsAligned(0, 6, 12), "0-6-12 are diagonally aligned");
    AssertTrue(game.IsAligned(6, 12, 18), "6-12-18 are diagonally aligned");
    AssertTrue(game.IsAligned(2, 8, 14), "2-8-14 are diagonally aligned");

    // Test diagonal alignments (top-right to bottom-left)
    AssertTrue(game.IsAligned(4, 8, 12), "4-8-12 are diagonally aligned");
    AssertTrue(game.IsAligned(8, 12, 16), "8-12-16 are diagonally aligned");
}

void TestIsAlignedNotAligned() {
    std::cout << "\n=== Test: IsAligned - Not Aligned ===" << std::endl;
    AdugoGame game;

    // Test non-aligned positions
    AssertFalse(game.IsAligned(0, 6, 11), "0-6-11 are not aligned");
    AssertFalse(game.IsAligned(0, 7, 14), "0-7-14 are not aligned");
    AssertFalse(game.IsAligned(5, 11, 18), "5-11-18 are not aligned");

    // Test with non-consecutive positions
    AssertFalse(game.IsAligned(0, 10, 20), "0-10-20 are not consecutive");
}

void TestIsAlignedSamePosition() {
    std::cout << "\n=== Test: IsAligned - Same Position ===" << std::endl;
    AdugoGame game;

    // Test with duplicate positions
    AssertFalse(game.IsAligned(6, 6, 12),
                "Cannot align with duplicate position");
    AssertFalse(game.IsAligned(6, 12, 12),
                "Cannot align with duplicate position");
    AssertFalse(game.IsAligned(6, 12, 6),
                "Cannot align with duplicate position");
}

void TestIsAlignedTriangleArea() {
    std::cout << "\n=== Test: IsAligned - Triangle Area ===" << std::endl;
    AdugoGame game;

    // Test alignments in triangle area
    AssertTrue(game.IsAligned(22, 26, 30), "22-26-30 are aligned");
    AssertTrue(game.IsAligned(22, 27, 32), "22-27-32 are aligned");
    AssertTrue(game.IsAligned(22, 28, 34), "22-28-34 are aligned");
    AssertTrue(game.IsAligned(26, 30, 32),
               "26-30-32 are aligned (if applicable)");
}

// ============================================================================
// TEST: GetPlayerToMove
// ============================================================================

void TestGetPlayerToMove() {
    std::cout << "\n=== Test: GetPlayerToMove ===" << std::endl;
    AdugoGame game;

    // Create states with different players
    Board board;
    board.fill(Symbol::kEmpty);

    State jaguar_state(board, Player(Symbol::kO));
    Player player = game.GetPlayerToMove(jaguar_state);
    AssertTrue(player.symbol == Symbol::kO, "Jaguar's turn");
    AssertFalse(player.is_max, "Jaguar is MIN player");

    State dog_state(board, Player(Symbol::kC));
    player = game.GetPlayerToMove(dog_state);
    AssertTrue(player.symbol == Symbol::kC, "Dogs' turn");
    AssertTrue(player.is_max, "Dogs are MAX player");
}

// ============================================================================
// TEST: GetActions
// ============================================================================

void TestGetActionsInitialState() {
    std::cout << "\n=== Test: GetActions - Initial State ===" << std::endl;
    AdugoGame game;

    State initial = game.GetInitialState();
    std::vector<Action> actions = game.GetActions(initial);

    AssertTrue(actions.size() > 0, "Jaguar has moves in initial state");

    // All actions should be for jaguar (Symbol::kO)
    bool all_jaguar = true;
    for (const auto& action : actions) {
        if (action.player_symbol != Symbol::kO) {
            all_jaguar = false;
            break;
        }
    }
    AssertTrue(all_jaguar, "All initial actions are jaguar moves");
}

void TestGetActionsEmptyBoard() {
    std::cout << "\n=== Test: GetActions - Empty Board ===" << std::endl;
    AdugoGame game;

    // Create empty board with single jaguar
    Board board;
    board.fill(Symbol::kEmpty);
    board[12] = Symbol::kO;  // Jaguar in center
    board[25] = Symbol::kBlock;
    board[29] = Symbol::kBlock;
    board[31] = Symbol::kBlock;
    board[33] = Symbol::kBlock;

    State state(board, Player(Symbol::kO));
    std::vector<Action> actions = game.GetActions(state);

    AssertTrue(actions.size() > 0, "Jaguar has moves on empty board");

    // Jaguar at position 12 has 8 neighbors
    AssertEqual(8, static_cast<int>(actions.size()),
                "Jaguar at center has 8 possible moves");
}

void TestGetActionsCaptureAvailable() {
    std::cout << "\n=== Test: GetActions - Capture Available ===" << std::endl;
    AdugoGame game;

    // Create scenario where jaguar can capture
    Board board;
    board.fill(Symbol::kEmpty);
    board[6] = Symbol::kO;  // Jaguar
    board[7] = Symbol::kC;  // Dog
    // Position 8 is empty - capture available
    board[25] = Symbol::kBlock;
    board[29] = Symbol::kBlock;
    board[31] = Symbol::kBlock;
    board[33] = Symbol::kBlock;

    State state(board, Player(Symbol::kO));
    std::vector<Action> actions = game.GetActions(state);

    // Check if capture action exists (6 -> 8)
    bool has_capture = false;
    for (const auto& action : actions) {
        if (action.cell_index_origin == 6 &&
            action.cell_index_destination == 8) {
            has_capture = true;
            break;
        }
    }
    AssertTrue(has_capture, "Capture action is available");
}

// ============================================================================
// TEST: GetResult
// ============================================================================

void TestGetResultSimpleMove() {
    std::cout << "\n=== Test: GetResult - Simple Move ===" << std::endl;
    AdugoGame game;

    State initial = game.GetInitialState();

    // Move jaguar from 12 to 17
    Action move(Symbol::kO, 12, 17);
    std::unique_ptr<State> result = game.GetResult(initial, move);

    AssertTrue(result != nullptr, "Result state is valid");
    AssertTrue((*result)[12] == Symbol::kEmpty, "Origin is now empty");
    AssertTrue((*result)[17] == Symbol::kO, "Destination has jaguar");
    AssertTrue(result->player_to_move.symbol == Symbol::kC,
               "Turn switches to dogs");
}

void TestGetResultCapture() {
    std::cout << "\n=== Test: GetResult - Capture ===" << std::endl;
    AdugoGame game;

    // Create capture scenario
    Board board;
    board.fill(Symbol::kEmpty);
    board[6] = Symbol::kO;  // Jaguar
    board[7] = Symbol::kC;  // Dog to be captured
    board[25] = Symbol::kBlock;
    board[29] = Symbol::kBlock;
    board[31] = Symbol::kBlock;
    board[33] = Symbol::kBlock;

    State state(board, Player(Symbol::kO));

    // Capture dog at 7 by moving to 8
    Action capture(Symbol::kO, 6, 8);
    std::unique_ptr<State> result = game.GetResult(state, capture);

    AssertTrue(result != nullptr, "Result state is valid");
    AssertTrue((*result)[6] == Symbol::kEmpty, "Jaguar left origin");
    AssertTrue((*result)[7] == Symbol::kEmpty, "Dog was captured");
    AssertTrue((*result)[8] == Symbol::kO, "Jaguar at destination");
}

// ============================================================================
// TEST: Player Class
// ============================================================================

void TestPlayerClass() {
    std::cout << "\n=== Test: Player Class ===" << std::endl;

    Player jaguar(Symbol::kO);
    AssertTrue(jaguar.symbol == Symbol::kO, "Jaguar symbol is kO");
    AssertFalse(jaguar.is_max, "Jaguar is MIN player");

    Player dog(Symbol::kC);
    AssertTrue(dog.symbol == Symbol::kC, "Dog symbol is kC");
    AssertTrue(dog.is_max, "Dog is MAX player");

    AssertTrue(jaguar != dog, "Different players are not equal");

    Player another_jaguar(Symbol::kO);
    AssertTrue(jaguar == another_jaguar, "Same symbols are equal");
}

// ============================================================================
// TEST: GetJaguarPosition
// ============================================================================

void TestGetJaguarPosition() {
    std::cout << "\n=== Test: GetJaguarPosition ===" << std::endl;
    AdugoGame game;

    State initial = game.GetInitialState();
    int jaguar_pos = game.GetJaguarPosition(initial);

    AssertEqual(12, jaguar_pos, "Initial jaguar position is 12");
    AssertTrue(initial[jaguar_pos] == Symbol::kO,
               "Position 12 contains jaguar");

    // Test after move
    Action move(Symbol::kO, 12, 17);
    std::unique_ptr<State> result = game.GetResult(initial, move);
    jaguar_pos = game.GetJaguarPosition(*result);

    AssertEqual(17, jaguar_pos, "Jaguar moved to position 17");
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  ADUGO GAME UNIT TESTS" << std::endl;
    std::cout << "========================================" << std::endl;

    // IsNeighbor tests
    TestIsNeighborBasic();
    TestIsNeighborNonNeighbors();
    TestIsNeighborTriangleArea();
    TestIsNeighborInvalidPositions();

    // FindMiddlePosition tests
    TestFindMiddlePositionHorizontal();
    TestFindMiddlePositionVertical();
    TestFindMiddlePositionDiagonal();
    TestFindMiddlePositionNoMiddle();
    TestFindMiddlePositionSymmetry();

    // IsAligned tests
    TestIsAlignedHorizontal();
    TestIsAlignedVertical();
    TestIsAlignedDiagonal();
    TestIsAlignedNotAligned();
    TestIsAlignedSamePosition();
    TestIsAlignedTriangleArea();

    // GetPlayerToMove tests
    TestGetPlayerToMove();

    // GetActions tests
    TestGetActionsInitialState();
    TestGetActionsEmptyBoard();
    TestGetActionsCaptureAvailable();

    // GetResult tests
    TestGetResultSimpleMove();
    TestGetResultCapture();

    // Player class tests
    TestPlayerClass();

    // GetJaguarPosition tests
    TestGetJaguarPosition();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  ✓ ALL UNIT TESTS PASSED!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
