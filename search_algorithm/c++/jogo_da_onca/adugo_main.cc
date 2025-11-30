#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_set>

#include "algorithms/adversarial_search/adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/games/adugo_game.h"
#include "server/tabuleiro.h"
#include "tabuleiro_wrapper.h"

// Forward declaration from tabuleiro_wrapper.cc
std::pair<int, int> IndexToPosition(int index);

struct Args {
   public:
    char side;
    std::string ip;
    int port;
};

Args ParseArgs(int argc, char** argv);
void PrintUsage(const char* program_name);
std::unique_ptr<Action> SearchMove(adugo_game::AdugoGame& game,
                                   const adugo_game::State& state);

int main(int argc, char** argv) {
    using namespace adugo_game;
    constexpr int kServerResponseTimeout = 300;  // 5 min timeout
    constexpr int kMaxDepth = 20;  // A safe bet for the depth regarding time

    // Parse command-line arguments
    Args args = ParseArgs(argc, argv);

    // Display configuration
    std::cout << "Configuration:\n";
    std::cout << "  Side: " << args.side << "\n";
    std::cout << "  IP: " << args.ip << "\n";
    std::cout << "  Port: " << args.port << "\n\n";

    TabuleiroWrapper tabuleiro;
    tabuleiro.ConnectToServer(argc, argv);

    // Initialize game
    AdugoGame game(kMaxDepth);
    std::unordered_map<State, int> state_count_table;
    std::unordered_set<State> penalized_states;

    Player my_player;
    if (args.side == 'c')
        my_player = Player(Symbol::kC);
    else
        my_player = Player(Symbol::kO);

    std::cout << "Playing as: " << args.side << "\n\n";

    while (true) {
        // Receive current state from server
        std::cout << "=== Waiting for server response ===" << std::endl;

        State current_state;
        try {
            current_state = tabuleiro.ReceiveState(
                kServerResponseTimeout);  // Timeout, that usually happens in
                                          // draws
        } catch (const TimeoutException& e) {
            std::cerr << "\n=== TIMEOUT ===" << std::endl;
            std::cerr << e.what() << std::endl;
            std::cerr << "Exiting ..." << std::endl;
            return 1;
        }

        // Print state information
        std::cout << "\nReceived state from server:\n";
        std::cout << "Current turn: "
                  << static_cast<char>(current_state.player_to_move.symbol)
                  << "\n";
        game.PrintState(current_state);

        // Check if game is over
        if (game.IsTerminal(current_state)) {
            std::cout << "\n=== GAME OVER ===" << std::endl;
            Utility final_utility = game.GetUtility(current_state);
            if (final_utility > 0) {
                std::cout << "Result: I WON!" << std::endl;
            } else if (final_utility < 0) {
                std::cout << "Result: I LOST!" << std::endl;
            } else {
                std::cout << "Result: DRAW!" << std::endl;
            }
            break;
        }

        // Check if it's my turn
        if (current_state.player_to_move.symbol == my_player.symbol) {
            std::cout << "\n>>> MY TURN <<<" << std::endl;

            std::cout << "State received" << std::endl;
            game.PrintState(current_state);

            // For jaguar: collect consecutive captures to send as sequence
            // For dogs: just make one move
            std ::vector<Action> actions_sequence;
            bool is_capture = false;
            State temp_state;
            bool is_first_move = true;

            do {
                temp_state = current_state;

            // First move (always execute)
            std::cout << "Calculating move 1..." << std::endl;
                std::unique_ptr<Action> best_action =
                    SearchMove(game, temp_state);

                is_capture = game.IsCaptureMove(*best_action);

                if (is_first_move || is_capture) {
            // Apply the action
            std::unique_ptr<State> next_state =
                game.GetResult(temp_state, *best_action);
                    if (!next_state) {
                        std::cerr << "ERROR: Invalid action result!"
                                  << std::endl;
                        break;
                    }
                    temp_state = *next_state;

                    // If it's a penalized state, search another time to
                    // avoid repetition and expand the horizon
                    if (penalized_states.find(temp_state) !=
                        penalized_states.end()) {
                        std::cout << "\033[1;31mPenalized state detected! "
                                     "Searching for alternative move...\033[0m"
                                  << std::endl;
                        best_action = SearchMove(game, temp_state);
                    }

                    std::cout << "Transposition state value stored: "
                              << game.transposition_table[temp_state]
                              << std::endl;

                    actions_sequence.push_back(*best_action);
                }

                // If jaguar made a capture and still has turn, look for more
                // captures
            } while (is_capture && !game.IsTerminal(temp_state));

            if (actions_sequence.empty()) {
                std::cerr << "ERROR: No valid action found!" << std::endl;
                break;
            }

            // Update state count table and penalized states for repetition
            // detection
            auto it = state_count_table.find(temp_state);
            if (it != state_count_table.end()) {
                // Key exists
                state_count_table[temp_state] += 1;

                if (state_count_table[temp_state] >= 3) {
                    // Print in red
                    std::cout << "\033[1;31mWARNING: State repeated "
                                 ""
                              << state_count_table[temp_state]
                              << " times! Inserting state into "
                                 "penalized_states set.\033[0m"
                              << std::endl;

                    penalized_states.insert(temp_state);
                }

            } else
                // Key does not exist, initialize count to 1
                state_count_table[temp_state] = 1;

            // Send the action(s) to the server
            std::cout << "\033[1mSending " << actions_sequence.size()
                      << " move(s) to server...\033[0m" << std::endl;

            if (actions_sequence.size() == 1) {
                // Single move
                tabuleiro.SendAction(my_player, actions_sequence[0]);
            } else {
                // Multiple moves (capture sequence) - send all at once
                tabuleiro.SendActionSequence(my_player, actions_sequence);
            }

            std::cout << "Move(s) sent!\n" << std::endl;

            // Print resulting board
            std::cout << "Resulting state after my move(s):\n";
            game.PrintState(temp_state);
        } else {
            std::cout << "\n>>> OPPONENT'S TURN - Waiting... <<<\n"
                      << std::endl;
        }
    }

    return 0;
}

std::unique_ptr<Action> SearchMove(adugo_game::AdugoGame& game,
                                   const adugo_game::State& state) {
    std::unique_ptr<Action> best_action =
        adversarial_search_algorithm::HeuristicMinimaxSearch(
            game, state, game.transposition_table);
    game.transposition_table.clear();

    if (!best_action) {
        std::cerr << "ERROR: No valid action found!" << std::endl;
        return nullptr;
    }

    auto [from_row, from_col] = IndexToPosition(best_action->cell_index_origin);
    auto [to_row, to_col] =
        IndexToPosition(best_action->cell_index_destination);
    std::cout << "  Move: (" << from_row << "," << from_col << ") -> ("
              << to_row << "," << to_col << ")" << std::endl;

    return best_action;
}

void PrintUsage(const char* program_name) {
    std::cout << "Usage:\n";
    std::cout << "  " << program_name << " <side> [ip] [port]\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  side    Side to play with (o or c) [required]\n";
    std::cout
        << "  ip      IP or hostname of Redis server (default: 127.0.0.1)\n";
    std::cout << "  port    Port of Redis server (default: 10001)\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help    Display this help message\n";
}

Args ParseArgs(int argc, char** argv) {
    // Default values
    Args args;
    args.side = '\0';
    args.ip = "127.0.0.1";
    args.port = 10001;

    // Define long options (only help flag)
    static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                           {0, 0, 0, 0}};

    // Parse command-line options
    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "h", long_options, &option_index)) !=
           -1) {
        switch (opt) {
            case 'h':
                PrintUsage(argv[0]);
                std::exit(0);
            case '?':
                PrintUsage(argv[0]);
                std::exit(1);
            default:
                PrintUsage(argv[0]);
                std::exit(1);
        }
    }

    // Parse positional arguments
    if (optind >= argc) {
        std::cerr << "Error: side argument is required\n\n";
        PrintUsage(argv[0]);
        std::exit(1);
    }

    // First positional argument: side
    args.side = argv[optind][0];
    if (args.side != 'o' && args.side != 'c') {
        std::cerr << "Error: side must be 'o' or 'c'\n\n";
        PrintUsage(argv[0]);
        std::exit(1);
    }
    optind++;

    // Second positional argument: IP (optional)
    if (optind < argc) {
        args.ip = argv[optind];
        optind++;
    }

    // Third positional argument: port (optional)
    if (optind < argc) {
        args.port = std::atoi(argv[optind]);
        if (args.port <= 0 || args.port > 65535) {
            std::cerr << "Error: invalid port number\n\n";
            PrintUsage(argv[0]);
            std::exit(1);
        }
        optind++;
    }

    return args;
}