#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string>

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

int main(int argc, char** argv) {
    using namespace adugo_game;

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
    AdugoGame game;
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
        State current_state = tabuleiro.ReceiveState();

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

            // Calculate best move using minimax
            std::cout << "Calculating best move..." << std::endl;
            std::unique_ptr<Action> best_action =
                adversarial_search_algorithm::HeuristicMinimaxSearch(
                    game, current_state, transposition_table);

            if (!best_action) {
                std::cerr << "ERROR: No valid action found!" << std::endl;
                break;
            }

            // Display the move
            auto [from_row, from_col] =
                IndexToPosition(best_action->cell_index_origin);
            auto [to_row, to_col] =
                IndexToPosition(best_action->cell_index_destination);
            std::cout << "\033[1mBest move: (" << from_row << "," << from_col
                      << ") -> (" << to_row << "," << to_col << ")\033[0m"
                      << std::endl;

            // Send the action to the server
            std::cout << "Sending move to server..." << std::endl;
            tabuleiro.SendAction(my_player, *best_action);
            std::cout << "Move sent!\n" << std::endl;
        } else {
            std::cout << "\n>>> OPPONENT'S TURN - Waiting... <<<\n"
                      << std::endl;
        }
    }

    return 0;
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