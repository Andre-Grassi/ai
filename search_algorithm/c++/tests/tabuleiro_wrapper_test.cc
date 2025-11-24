#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "data_structure/adversarial_search/games/adugo_game.h"

using namespace adugo_game;

// Mock tabuleiro_envia to capture the buffer
static std::string captured_buffer;
extern "C" {
void tabuleiro_envia(char* buf) { captured_buffer = std::string(buf); }

void tabuleiro_recebe(char* buf) {
    // Mock implementation - not used in these tests
    (void)buf;
}
}

// Forward declare the helper functions from tabuleiro_wrapper.cc
std::pair<int, int> IndexToPosition(int index);

// Test helper to create message buffer for simple action
std::string CreateSimpleActionBuffer(const Player& player,
                                     const Action& action) {
    char player_char;
    if (player.symbol == Symbol::kC)
        player_char = 'c';
    else if (player.symbol == Symbol::kO)
        player_char = 'o';
    else
        throw std::invalid_argument("Invalid player symbol");

    auto [row_origin, col_origin] = IndexToPosition(action.cell_index_origin);
    auto [row_destiny, col_destiny] =
        IndexToPosition(action.cell_index_destination);

    char movement_char = 'm';
    if (player.symbol == Symbol::kO) {
        int row_diff = std::abs(row_destiny - row_origin);
        int col_diff = std::abs(col_destiny - col_origin);
        if (row_diff > 1 || col_diff > 1) {
            movement_char = 's';
        }
    }

    char buf[512];
    snprintf(buf, sizeof(buf), "%c %c %d %d %d %d\n", player_char,
             movement_char, row_origin, col_origin, row_destiny, col_destiny);
    return std::string(buf);
}

// Test helper to create message buffer for action sequence
std::string CreateSequenceActionBuffer(const Player& player,
                                       const std::vector<Action>& actions) {
    if (actions.empty()) {
        throw std::invalid_argument("Action sequence cannot be empty");
    }

    char player_char;
    if (player.symbol == Symbol::kC)
        player_char = 'c';
    else if (player.symbol == Symbol::kO)
        player_char = 'o';
    else
        throw std::invalid_argument("Invalid player symbol");

    if (player.symbol != Symbol::kO) {
        throw std::invalid_argument("Only jaguar can make sequential moves");
    }

    char buf[512];
    int pos = 0;

    pos += snprintf(buf + pos, sizeof(buf) - pos, "%c s %zu", player_char,
                    actions.size());

    auto [row_origin, col_origin] =
        IndexToPosition(actions[0].cell_index_origin);
    pos += snprintf(buf + pos, sizeof(buf) - pos, " %d %d", row_origin,
                    col_origin);

    for (const auto& action : actions) {
        auto [row_dest, col_dest] =
            IndexToPosition(action.cell_index_destination);
        pos += snprintf(buf + pos, sizeof(buf) - pos, " %d %d", row_dest,
                        col_dest);
    }

    pos += snprintf(buf + pos, sizeof(buf) - pos, "\n");
    return std::string(buf);
}

void TestSimpleDogMove() {
    std::cout << "Test: Simple Dog Move (c)" << std::endl;

    // Dog moves from position 0 (row=1, col=1) to position 1 (row=1, col=2)
    Player dog(Symbol::kC);
    Action action(Symbol::kC, 0, 1);

    std::string expected = "c m 1 1 1 2\n";
    std::string actual = CreateSimpleActionBuffer(dog, action);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

void TestSimpleJaguarMove() {
    std::cout << "Test: Simple Jaguar Move (o)" << std::endl;

    // Jaguar moves from position 12 (row=3, col=3) to position 7 (row=2, col=3)
    Player jaguar(Symbol::kO);
    Action action(Symbol::kO, 12, 7);

    std::string expected = "o m 3 3 2 3\n";
    std::string actual = CreateSimpleActionBuffer(jaguar, action);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

void TestSingleJaguarCapture() {
    std::cout << "Test: Single Jaguar Capture (o s)" << std::endl;

    // Jaguar captures from position 2 (row=1, col=3) to position 12 (row=3,
    // col=3) This is detected as 's' in SendAction because distance > 1
    Player jaguar(Symbol::kO);
    Action action(Symbol::kO, 2, 12);

    std::string expected = "o s 1 3 3 3\n";
    std::string actual = CreateSimpleActionBuffer(jaguar, action);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

void TestDoubleJaguarCapture() {
    std::cout << "Test: Double Jaguar Capture Sequence" << std::endl;

    // Jaguar makes two captures:
    // 1. From position 2 (row=1, col=3) to position 12 (row=3, col=3)
    // 2. From position 12 (row=3, col=3) to position 22 (row=5, col=3)
    Player jaguar(Symbol::kO);
    std::vector<Action> actions = {Action(Symbol::kO, 2, 12),
                                   Action(Symbol::kO, 12, 22)};

    // Format: o s <num_captures> <row0> <col0> <row1> <col1> <row2> <col2>
    // num_captures = 2, positions: (1,3) -> (3,3) -> (5,3)
    std::string expected = "o s 2 1 3 3 3 5 3\n";
    std::string actual = CreateSequenceActionBuffer(jaguar, actions);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

void TestTripleJaguarCapture() {
    std::cout << "Test: Triple Jaguar Capture Sequence" << std::endl;

    // Jaguar makes three captures:
    // Position 12 (3,3) -> 16 (4,2) -> 10 (3,1) -> 0 (1,1)
    Player jaguar(Symbol::kO);
    std::vector<Action> actions = {Action(Symbol::kO, 12, 16),
                                   Action(Symbol::kO, 16, 10),
                                   Action(Symbol::kO, 10, 0)};

    // Format: o s 3 3 3 4 2 3 1 1 1
    std::string expected = "o s 3 3 3 4 2 3 1 1 1\n";
    std::string actual = CreateSequenceActionBuffer(jaguar, actions);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

void TestDogMoveCorner() {
    std::cout << "Test: Dog Move from Corner" << std::endl;

    // Dog moves from position 0 (row=1, col=1) to position 5 (row=2, col=1)
    Player dog(Symbol::kC);
    Action action(Symbol::kC, 0, 5);

    std::string expected = "c m 1 1 2 1\n";
    std::string actual = CreateSimpleActionBuffer(dog, action);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

void TestJaguarMoveTriangle() {
    std::cout << "Test: Jaguar Move in Triangle Area" << std::endl;

    // Jaguar moves from position 22 (row=5, col=3) to position 26 (row=6,
    // col=2)
    Player jaguar(Symbol::kO);
    Action action(Symbol::kO, 22, 26);

    std::string expected = "o m 5 3 6 2\n";
    std::string actual = CreateSimpleActionBuffer(jaguar, action);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

void TestJaguarCaptureWithTriangle() {
    std::cout << "Test: Jaguar Capture Involving Triangle" << std::endl;

    // Jaguar captures from position 12 (row=3, col=3) to position 22 (row=5,
    // col=3) to position 32 (row=7, col=3)
    Player jaguar(Symbol::kO);
    std::vector<Action> actions = {Action(Symbol::kO, 12, 22),
                                   Action(Symbol::kO, 22, 32)};

    std::string expected = "o s 2 3 3 5 3 7 3\n";
    std::string actual = CreateSequenceActionBuffer(jaguar, actions);

    std::cout << "  Expected: " << expected;
    std::cout << "  Actual:   " << actual;

    assert(expected == actual);
    std::cout << "  ✓ PASSED\n" << std::endl;
}

int main() {
    std::cout << "\n=== Tabuleiro Wrapper Buffer Tests ===\n" << std::endl;

    try {
        // Simple movement tests
        TestSimpleDogMove();
        TestSimpleJaguarMove();
        TestDogMoveCorner();
        TestJaguarMoveTriangle();

        // Single capture tests
        TestSingleJaguarCapture();

        // Sequential capture tests
        TestDoubleJaguarCapture();
        TestTripleJaguarCapture();
        TestJaguarCaptureWithTriangle();

        std::cout << "=== All Tests Passed! ===\n" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
}
