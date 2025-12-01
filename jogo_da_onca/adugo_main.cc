/**
 * @file adugo_main.cc
 * @brief Main program for playing Adugo game with AI agent using Minimax
 * algorithm
 * @author Andre Grassi, Caue Samonek, Ricardo Faria
 * @date 2025
 */

#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_set>

#include "algorithms/adversarial_search/adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/games/adugo_game.h"
#include "server/tabuleiro.h"
#include "tabuleiro_wrapper.h"

// Used to parse command-line arguments
struct Args {
   public:
    char side;
    int max_moves;
    std::string ip;
    int port;
};

/*
 * Program usage functions
 */
Args ParseArgs(int argc, char** argv);
void PrintUsage(const char* program_name);

/*
 * Game playing utilities
 */

// Forward declaration from tabuleiro_wrapper.cc
std::pair<int, int> IndexToPosition(int index);

// Get the best move with heuristic minimax search from the given state
std::unique_ptr<Action> SearchMove(adugo_game::AdugoGame& game,
                                   const adugo_game::State& state);

// Send a sequence of actions to the server.
// Usually the sequence is just one action, except for jaguar captures.
// which can be multiple jumps in one turn.
void SendActionsToServer(const Player& player,
                         std::vector<Action> actions_sequence,
                         TabuleiroWrapper& tabuleiro);

int main(int argc, char** argv) {
    using namespace adugo_game;
    constexpr int kServerResponseTimeout =
        300;  // 5 min timeout of server response
    constexpr int kMaxDepth =
        12;  // A safe bet for the minimax search tree depth regarding time

    // Parse command-line arguments
    Args args = ParseArgs(argc, argv);

    // Display configuration
    std::cout << "Configuration:\n";
    std::cout << "  Side: " << args.side << "\n";
    std::cout << "  Max Moves: " << args.max_moves << "\n";
    std::cout << "  IP: " << args.ip << "\n";
    std::cout << "  Port: " << args.port << "\n\n";

    // Adapter from tabuleiro lib to our data structures
    TabuleiroWrapper tabuleiro;
    tabuleiro.ConnectToServer(
        args.side, const_cast<char*>(args.ip.c_str()),
        const_cast<char*>(std::to_string(args.port).c_str()));

    // Initialize game
    AdugoGame game(kMaxDepth);

    // Table to count state occurrences for repetition detection
    std::unordered_map<State, int> state_count_table;

    // Set of penalized states to avoid repetition.
    // When a state is repeated more than once, it is added here.
    // Then, when reaching a penalized state, the agent will perform a new
    // search, to try to look beyond and find more promising moves.
    std::unordered_set<State> penalized_states;

    Player my_player;
    if (args.side == 'c')
        my_player = Player(Symbol::kC);
    else
        my_player = Player(Symbol::kO);

    std::cout << "Playing as: " << args.side << "\n\n";

    int moves = 0;
    while (moves <= args.max_moves / 2) {
        std::cout << "MOVES=" << std::to_string(moves) << std::endl;
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

        moves++;

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
            bool is_capture = false;  // To track if the last move was a capture
            State temp_state = current_state;
            bool is_first_move = true;

            do {
                std::cout << "Calculating move..." << std::endl;

                std::unique_ptr<Action> best_action =
                    SearchMove(game, temp_state);

                is_capture = game.IsCaptureMove(*best_action);

                // Check if it's the first move (to always apply it) or a
                // capture in case the jaguar is performing consecutive
                // captures.
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
                                     "Expanding search...\033[0m"
                                  << std::endl;
                        best_action = SearchMove(game, temp_state);
                    }

                    // Store action in sequence
                    actions_sequence.push_back(*best_action);
                }

                is_first_move = false;

                // If jaguar made a capture and still has turn, look for more
                // captures.
                // Otherwise, stop processing our moves.
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

                if (state_count_table[temp_state] >= 2) {
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

            SendActionsToServer(my_player, actions_sequence, tabuleiro);

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

void SendActionsToServer(const Player& player,
                         std::vector<Action> actions_sequence,
                         TabuleiroWrapper& tabuleiro) {
    // Send the action(s) to the server
    std::cout << "\033[1mSending " << actions_sequence.size()
              << " move(s) to server...\033[0m" << std::endl;

    if (actions_sequence.size() == 1) {
        // Single move
        tabuleiro.SendAction(player, actions_sequence[0]);
    } else {
        // Multiple moves (capture sequence) - send all at once
        tabuleiro.SendActionSequence(player, actions_sequence);
    }

    std::cout << "Move(s) sent!\n" << std::endl;
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
    std::cout << "  max_moves  Maximum number of moves to play (positive "
                 "integer) [required]\n";
    std::cout << "  ip      IP or hostname of Redis server (default: "
                 "127.0.0.1)\n";
    std::cout << "  port    Port of Redis server (default: 10001)\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help    Display this help message\n";
}

Args ParseArgs(int argc, char** argv) {
    // Default values
    Args args;
    args.side = '\0';
    args.max_moves = -1;
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

    // Second positional argument: max moves
    std::cout << "optind: " << optind << " argc: " << argc << std::endl;
    if (optind < argc) {
        args.max_moves = std::atoi(argv[optind]);
        if (args.max_moves <= -1) {
            std::cerr << "Error: invalid max moves number\n\n";
            PrintUsage(argv[0]);
            std::exit(1);
        }
        optind++;
    } else {
        std::cerr << "Error: max moves argument is required\n\n";
        PrintUsage(argv[0]);
        std::exit(1);
    }

    // Third positional argument: IP (optional)
    if (optind < argc) {
        args.ip = argv[optind];
        optind++;
    }

    // Fourth positional argument: port (optional)
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