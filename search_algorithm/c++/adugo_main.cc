#include <iostream>

#include "algorithms/adversarial_search/adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/games/adugo_game.h"

int main() {
    using namespace adugo_game;

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
