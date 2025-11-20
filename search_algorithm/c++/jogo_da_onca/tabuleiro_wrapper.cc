#include "tabuleiro_wrapper.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "data_structure/adversarial_search/games/adugo_game.h"
#include "server/tabuleiro.h"

using namespace adugo_game;

Symbol CharToPlayerSymbol(char c) {
    switch (c) {
        case 'c':
            return Symbol::kC;
        case 'o':
            return Symbol::kO;
        default:
            throw std::invalid_argument("Invalid player character");
    }
}

char PlayerSymbolToChar(Symbol symbol) {
    switch (symbol) {
        case Symbol::kC:
            return 'c';
        case Symbol::kO:
            return 'o';
        default:
            throw std::invalid_argument("Invalid player symbol");
    }
}

// Helper function to convert 1D index to 2D position (1-indexed)
std::pair<int, int> IndexToPosition(int index) {
    // The board layout maps indices 0-34 to positions (row, col)
    // Row 1: indices 0-4   -> (1, 1-5)
    // Row 2: indices 5-9   -> (2, 1-5)
    // Row 3: indices 10-14 -> (3, 1-5)
    // Row 4: indices 15-19 -> (4, 1-5)
    // Row 5: indices 20-24 -> (5, 1-5)
    // Row 6: indices 26,27,28 -> (6, 2,3,4) [triangle top]
    // Row 7: indices 30,32,34 -> (7, 2,3,4) [triangle bottom]

    if (index >= 0 && index <= 24) {
        // Regular grid: rows 1-5
        int row = (index / kBoardWidth) + 1;  // 1-indexed row
        int col = (index % kBoardWidth) + 1;  // 1-indexed column
        return {row, col};
    } else if (index == 26) {
        return {6, 2};
    } else if (index == 27) {
        return {6, 3};
    } else if (index == 28) {
        return {6, 4};
    } else if (index == 30) {
        return {7, 2};
    } else if (index == 32) {
        return {7, 3};
    } else if (index == 34) {
        return {7, 4};
    } else {
        throw std::invalid_argument("Invalid board index: " +
                                    std::to_string(index));
    }
}

// Helper function to convert 2D position (1-indexed) to 1D index
int PositionToIndex(int row, int col) {
    // Row 1: (1, 1-5) -> indices 0-4
    // Row 2: (2, 1-5) -> indices 5-9
    // Row 3: (3, 1-5) -> indices 10-14
    // Row 4: (4, 1-5) -> indices 15-19
    // Row 5: (5, 1-5) -> indices 20-24
    // Row 6: (6, 2,3,4) -> indices 26,27,28 [triangle top]
    // Row 7: (7, 2,3,4) -> indices 30,32,34 [triangle bottom]

    if (row >= 1 && row <= 5 && col >= 1 && col <= 5) {
        // Regular grid: rows 1-5, cols 1-5
        return (row - 1) * kBoardWidth + (col - 1);
    } else if (row == 6) {
        // Triangle top
        if (col == 2) return 26;
        if (col == 3) return 27;
        if (col == 4) return 28;
    } else if (row == 7) {
        // Triangle bottom
        if (col == 2) return 30;
        if (col == 3) return 32;
        if (col == 4) return 34;
    }

    throw std::invalid_argument("Invalid position: (" + std::to_string(row) +
                                ", " + std::to_string(col) + ")");
}

void TabuleiroWrapper::SendAction(const Player& player, const Action& action) {
    char player_char;
    if (player.symbol == Symbol::kC)
        player_char = 'c';
    else if (player.symbol == Symbol::kO)
        player_char = 'o';
    else
        throw std::invalid_argument("Invalid player symbol");

    // Convert indices to positions
    auto [row_origin, col_origin] = IndexToPosition(action.cell_index_origin);
    auto [row_destiny, col_destiny] =
        IndexToPosition(action.cell_index_destination);

    // Determine movement type
    char movement_char = 'm';  // default to simple move

    // Check if it's a capture (jaguar jumping over a dog)
    if (player.symbol == Symbol::kO) {
        // If the distance is more than 1 neighbor, it's likely a capture
        int row_diff = std::abs(row_destiny - row_origin);
        int col_diff = std::abs(col_destiny - col_origin);

        // If moved more than 1 step in any direction, it's a capture sequence
        if (row_diff > 1 || col_diff > 1) {
            movement_char = 's';  // capture sequence
        }
    }

    // Format the message for the server
    char buf[512];
    snprintf(buf, sizeof(buf), "%c %c %d %d %d %d\n", player_char,
             movement_char, row_origin, col_origin, row_destiny, col_destiny);

    // Send to server
    tabuleiro_envia(buf);
}

Action TabuleiroWrapper::ReceiveAction() {
    char buf[512];
    tabuleiro_recebe(buf);

    constexpr int max_int = 16;

    char my_player_char;
    char opponent_player_char;
    char opponent_move_char;
    int num_opponent_move;
    int opponent_move_r[max_int];
    int opponent_move_c[max_int];

    // separa os elementos do string recebido
    sscanf(strtok(buf, " \n"), "%c", &my_player_char);
    sscanf(strtok(NULL, " \n"), "%c", &opponent_player_char);
    sscanf(strtok(NULL, " \n"), "%c", &opponent_move_char);

    if (opponent_move_char == 'n') return Action();  // No move made by opponent

    if (opponent_move_char == 'm') {
        num_opponent_move = 1;
        for (int i = 0; i <= num_opponent_move; i++) {
            sscanf(strtok(NULL, " \n"), "%d", &(opponent_move_r[i]));
            sscanf(strtok(NULL, " \n"), "%d", &(opponent_move_c[i]));
        }
    } else if (opponent_move_char == 's') {
        sscanf(strtok(NULL, " \n"), "%d", &num_opponent_move);
        for (int i = 0; i <= num_opponent_move; i++) {
            sscanf(strtok(NULL, " \n"), "%d", &(opponent_move_r[i]));
            sscanf(strtok(NULL, " \n"), "%d", &(opponent_move_c[i]));
        }
    }

    Symbol opponent = CharToPlayerSymbol(opponent_player_char);

    // Convert opponent's move from positions to indices
    // TODO if move is s, need to handle multiple jumps
    int cell_index_origin =
        PositionToIndex(opponent_move_r[0], opponent_move_c[0]);
    int cell_index_destination = PositionToIndex(
        opponent_move_r[num_opponent_move], opponent_move_c[num_opponent_move]);

    return Action(opponent, cell_index_origin, cell_index_destination);
}