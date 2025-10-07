#include <iostream>
#include <map>
#include <utility>

#include "algorithms/adversarial_search/adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/games/tic_tac_toe_game.h"

int main() {
    using namespace tic_tac_toe_game;

    TicTacToeGame game;
    game.PrintState(game.GetInitialState());

    bool x_turn = true;
    std::unique_ptr<Action> best_action;
    std::map<int, std::vector<State>> levels;
    Utility expected_value;
    State current_state = game.GetInitialState();
    while (!game.IsTerminal(current_state)) {
        std::cout << std::endl;

        std::tie<std::unique_ptr<Action>, std::map<int, std::vector<State>>,
                 Utility>(best_action, levels, expected_value) =
            adversarial_search_algorithm::MinimaxSearchStoreTree(game,
                                                                 current_state);

        char player_char = 'X';
        if (!x_turn) player_char = 'O';
        x_turn = !x_turn;

        // Output the entire map in the form Depth:<depth>\n<state0>\n<state1>
        for (const auto& [depth, states] : levels) {
            std::cout << std::to_string(depth) + " (" + player_char + "): "
                      << std::endl;
            for (State state : states)
                std::cout << game.GetStateString(state) << std::endl;
        }

        // Output chosen action and the expected min utility (for max player) or
        // max utility (for min player)
        std::cout << "Chosen action: " << player_char << " at "
                  << std::to_string(best_action->cell_index) << std::endl;
        std::cout << "Expected value: " << expected_value << std::endl;

        std::unique_ptr<State> next_state =
            game.GetResult(current_state, *best_action);
        current_state = *next_state;
    }

    return 0;
}
