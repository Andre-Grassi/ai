#include "tabuleiro_wrapper.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <thread>

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

void TabuleiroWrapper::SendActionSequence(const Player& player,
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

    // Only jaguar can make sequential captures
    if (player.symbol != Symbol::kO) {
        throw std::invalid_argument("Only jaguar can make sequential moves");
    }

    // Format: o s <num_captures> <row0> <col0> <row1> <col1> ... <rowN> <colN>
    // where num_captures is the number of jumps (actions.size())
    // and we send num_captures+1 positions (origin + all destinations)
    char buf[512];
    int pos = 0;

    // Start with player, movement type, and number of captures
    pos += snprintf(buf + pos, sizeof(buf) - pos, "%c s %zu", player_char,
                    actions.size());

    // Add origin position of first action
    auto [row_origin, col_origin] =
        IndexToPosition(actions[0].cell_index_origin);
    pos += snprintf(buf + pos, sizeof(buf) - pos, " %d %d", row_origin,
                    col_origin);

    // Add destination of each action in the sequence
    for (const auto& action : actions) {
        auto [row_dest, col_dest] =
            IndexToPosition(action.cell_index_destination);
        pos += snprintf(buf + pos, sizeof(buf) - pos, " %d %d", row_dest,
                        col_dest);
    }

    pos += snprintf(buf + pos, sizeof(buf) - pos, "\n");

    // Send to server
    tabuleiro_envia(buf);
}

State TabuleiroWrapper::ReceiveState(int timeout_seconds) {
    char buf[512];

    // Use atomic flag and thread to implement timeout
    std::atomic<bool> received(false);
    std::atomic<bool> should_stop(false);

    std::thread receiver_thread([&buf, &received, &should_stop]() {
        if (!should_stop.load()) {
            tabuleiro_recebe(buf);
            received.store(true);
        }
    });

    // Wait for timeout or completion
    auto start = std::chrono::steady_clock::now();
    while (!received.load()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::steady_clock::now() - start)
                           .count();

        if (elapsed >= timeout_seconds) {
            should_stop.store(true);
            receiver_thread.detach();  // Detach thread as we can't cancel it
            throw TimeoutException("No response from server after " +
                                   std::to_string(timeout_seconds) +
                                   " seconds");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    receiver_thread.join();

    // Parse using strtok - need to be careful about order
    // Format: <my_side>\n<opponent_side> <move_type> [positions]\n<board>.\n

    // First line: whose turn it is
    char* line1 = strtok(buf, "\n");
    if (!line1) {
        throw std::runtime_error("Invalid server response: missing first line");
    }
    char whose_turn = line1[0];

    // Second line: opponent's last move
    char* line2 = strtok(NULL, "\n");
    if (!line2) {
        throw std::runtime_error(
            "Invalid server response: missing second line");
    }

    // Get the rest as board string (everything until '.')
    char board_buffer[512] = "";
    char* next_line;
    while ((next_line = strtok(NULL, "\n")) != NULL) {
        strcat(board_buffer, next_line);
        strcat(board_buffer, "\n");
    }

    // Determine whose turn it is
    Player player_to_move;
    if (whose_turn == 'c')
        player_to_move = Player(Symbol::kC);
    else if (whose_turn == 'o')
        player_to_move = Player(Symbol::kO);
    else
        throw std::invalid_argument("Invalid turn character: " +
                                    std::string(1, whose_turn));

    State server_state =
        GetStateFromBoardString(std::string(board_buffer), player_to_move);

    return server_state;
}

State TabuleiroWrapper::GetStateFromBoardString(const std::string& server_board,
                                                const Player& player_to_move) {
    Board board_array;

    // Initialize board with blocks
    for (size_t i = 0; i < kGridDimension; ++i) {
        board_array[i] = Symbol::kBlock;
    }

    // Fill the board array
    size_t board_pos = 0;

    for (size_t i = 0; i < server_board.size() && board_pos < kGridDimension;
         ++i) {
        char c = server_board[i];
        switch (c) {
            case 'c':
                board_array[board_pos++] = Symbol::kC;
                break;
            case 'o':
                board_array[board_pos++] = Symbol::kO;
                break;
            case '-':
                board_array[board_pos++] = Symbol::kEmpty;
                break;
            case ' ':
                board_array[board_pos++] = Symbol::kBlock;
                break;
            case '\n':
            case '#':
            case '\r':
            case '.':
                // Ignore border characters, newlines, and period
                break;
            default:
                // Ignore other characters
                break;
        }
    }

    if (board_pos != kGridDimension) {
        std::cerr << "Warning: Expected " << kGridDimension
                  << " positions but got " << board_pos << std::endl;
    }

    return State(board_array, player_to_move);
}