#include <getopt.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "../jogo_da_onca/server/tabuleiro.h"
#include "../jogo_da_onca/tabuleiro_wrapper.h"
#include "algorithms/adversarial_search/adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/games/adugo_game.h"

// Forward declaration from tabuleiro_wrapper.cc
std::pair<int, int> IndexToPosition(int index);

struct Args {
   public:
    char side;
    std::string ip;
    int port;
    int depth;
};

Args ParseArgs(int argc, char** argv);
void AdjustArgv(Args args, int argc, char** argv, int& new_argc,
                char**& new_argv);
void PrintUsage(const char* program_name);

int main(int argc, char** argv) {
    using namespace adugo_game;
    constexpr int kServerResponseTimeout = 20;

    // Parse command-line arguments
    Args args = ParseArgs(argc, argv);

    if (args.depth == -1)
        args.depth = AdugoGame::kMaxDepth;  // Use default if not set

    // Display configuration
    std::cout << "Configuration:\n";
    std::cout << "  Side: " << args.side << "\n";
    std::cout << "  Max Depth: " << args.depth << "\n";
    std::cout << "  IP: " << args.ip << "\n";
    std::cout << "  Port: " << args.port << "\n\n";

    // Remove depth from argv if it was provided, to pass correct argv to
    // tabuleiro
    int adjusted_argc;
    char** adjusted_argv;
    AdjustArgv(args, argc, argv, adjusted_argc, adjusted_argv);

    TabuleiroWrapper tabuleiro;
    tabuleiro.ConnectToServer(adjusted_argc, adjusted_argv);

    // Initialize game
    AdugoGame game(args.depth);
    std::unordered_map<State, Utility> transposition_table;

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
                kServerResponseTimeout);  // Timeout, that usually happens
                                          // in draws
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

            // For jaguar: collect consecutive captures to send as sequence
            // For dogs: just make one move
            std::vector<Action> actions_sequence;
            State temp_state = current_state;

            // First move (always execute)
            std::cout << "Calculating move 1..." << std::endl;
            std::unique_ptr<Action> best_action =
                adversarial_search_algorithm::HeuristicMinimaxSearch(
                    game, temp_state, transposition_table);

            if (!best_action) {
                std::cerr << "ERROR: No valid action found!" << std::endl;
                break;
            }

            auto [from_row, from_col] =
                IndexToPosition(best_action->cell_index_origin);
            auto [to_row, to_col] =
                IndexToPosition(best_action->cell_index_destination);
            std::cout << "  Move 1: (" << from_row << "," << from_col
                      << ") -> (" << to_row << "," << to_col << ")"
                      << std::endl;

            // Check if this first move is a capture
            bool is_capture = false;
            if (my_player.symbol == Symbol::kO) {
                is_capture =
                    !game.IsNeighbor(best_action->cell_index_origin,
                                     best_action->cell_index_destination);
            }

            actions_sequence.push_back(*best_action);

            // Apply the action
            std::unique_ptr<State> next_state =
                game.GetResult(temp_state, *best_action);
            if (!next_state) {
                std::cerr << "ERROR: Invalid action result!" << std::endl;
                break;
            }
            temp_state = *next_state;

            // If jaguar made a capture and still has turn, look for more
            // captures
            if (my_player.symbol == Symbol::kO && is_capture &&
                temp_state.player_to_move.symbol == my_player.symbol &&
                !game.IsTerminal(temp_state)) {
                // Keep looking for consecutive captures
                while (true) {
                    std::cout << "Calculating move "
                              << (actions_sequence.size() + 1) << "..."
                              << std::endl;

                    best_action =
                        adversarial_search_algorithm::HeuristicMinimaxSearch(
                            game, temp_state, transposition_table);

                    if (!best_action) {
                        std::cerr << "ERROR: No valid action found!"
                                  << std::endl;
                        break;
                    }

                    // Check if this next move is also a capture
                    bool next_is_capture =
                        !game.IsNeighbor(best_action->cell_index_origin,
                                         best_action->cell_index_destination);

                    std::tie(from_row, from_col) =
                        IndexToPosition(best_action->cell_index_origin);
                    std::tie(to_row, to_col) =
                        IndexToPosition(best_action->cell_index_destination);
                    std::cout << "  Move " << (actions_sequence.size() + 1)
                              << ": (" << from_row << "," << from_col
                              << ") -> (" << to_row << "," << to_col << ")";

                    // Only continue the sequence if it's another capture
                    if (!next_is_capture) {
                        std::cout << " (not a capture, ending sequence)"
                                  << std::endl;
                        break;
                    }

                    std::cout << std::endl;
                    actions_sequence.push_back(*best_action);

                    // Apply this capture
                    next_state = game.GetResult(temp_state, *best_action);
                    if (!next_state) {
                        std::cerr << "ERROR: Invalid action result!"
                                  << std::endl;
                        break;
                    }
                    temp_state = *next_state;

                    // Stop if it's no longer our turn or game ended
                    if (temp_state.player_to_move.symbol != my_player.symbol ||
                        game.IsTerminal(temp_state)) {
                        break;
                    }
                }
            }

            if (actions_sequence.empty()) {
                std::cerr << "ERROR: No valid action found!" << std::endl;
                break;
            }

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
        } else {
            std::cout << "\n>>> OPPONENT'S TURN - Waiting... <<<\n"
                      << std::endl;
        }
    }

    // Cleanup allocated argv
    delete[] adjusted_argv[1];
    delete[] adjusted_argv[2];
    delete[] adjusted_argv[3];
    delete[] adjusted_argv;

    return 0;
}

void PrintUsage(const char* program_name) {
    std::cout << "Usage:\n";
    std::cout << "  " << program_name << " <side> [depth] [ip] [port]\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  side    Side to play with (o or c) [required]\n";
    std::cout << "  depth   Max depth for minimax search (default: 10)\n";
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
    args.depth = -1;  // Indicates not set
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

    // Second positional argument: depth (optional)
    if (optind < argc) {
        args.depth = std::atoi(argv[optind]);
        if (args.depth <= 0) {
            std::cerr << "Error: depth must be a positive integer\n\n";
            PrintUsage(argv[0]);
            std::exit(1);
        }
        optind++;
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

void AdjustArgv(Args args, int argc, char** argv, int& new_argc,
                char**& new_argv) {
    // Construct argv for tabuleiro (without depth parameter)
    // tabuleiro expects: [program, side, ip, port]
    char** adjusted_argv = new char*[4];
    adjusted_argv[0] = argv[0];  // program name
    adjusted_argv[1] = new char[2];
    adjusted_argv[1][0] = args.side;
    adjusted_argv[1][1] = '\0';

    // IP
    adjusted_argv[2] = new char[args.ip.length() + 1];
    std::strcpy(adjusted_argv[2], args.ip.c_str());

    // Port
    std::string port_str = std::to_string(args.port);
    adjusted_argv[3] = new char[port_str.length() + 1];
    std::strcpy(adjusted_argv[3], port_str.c_str());

    int adjusted_argc = 4;

    new_argc = adjusted_argc;
    new_argv = adjusted_argv;
}