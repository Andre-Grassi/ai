#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "algorithms/adversarial_search/adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/games/adugo_game.h"
#include "server/tabuleiro.h"
#include "tabuleiro_wrapper.h"

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
    while (true) {
        Action action = tabuleiro.ReceiveAction();
        // Print action information
        std::cout << "Received action from server:\n";
        std::cout << "  Player Symbol: "
                  << static_cast<char>(action.player_symbol) << "\n";
        std::cout << "  Origin Index: " << action.cell_index_origin << "\n";
        std::cout << "  Destination Index: " << action.cell_index_destination
                  << "\n";
    }

    return 0;

    // Initialize game
    AdugoGame game;
    game.PrintState(game.GetInitialState());

    std::unordered_map<State, Utility> transposition_table;
    State current_state = game.GetInitialState();

    while (!game.IsTerminal(current_state)) {
        std::cout << std::endl;

        std::unique_ptr<Action> best_action =
            adversarial_search_algorithm::HeuristicMinimaxSearch(
                game, current_state, transposition_table);

        std::unique_ptr<State> next_state =
            game.GetResult(current_state, *best_action);
        current_state = *next_state;

        game.PrintState(current_state);
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