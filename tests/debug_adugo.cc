#include <iostream>

#include "data_structure/adversarial_search/games/adugo_game.h"

using namespace adugo_game;

int main() {
    AdugoGame game;
    State state = game.GetInitialState();

    std::cout << "Initial state:" << std::endl;
    game.PrintState(state);

    // Get initial actions
    std::vector<Action> actions = game.GetActions(state);
    std::cout << "\nJaguar has " << actions.size()
              << " actions from initial state" << std::endl;

    if (actions.empty()) {
        std::cerr << "ERROR: No actions available!" << std::endl;
        return 1;
    }

    // Make a move
    std::cout << "\nMaking first move: " << actions[0].cell_index_origin
              << " -> " << actions[0].cell_index_destination << std::endl;
    std::unique_ptr<State> state2 = game.GetResult(state, actions[0]);

    std::cout << "\nState after jaguar move:" << std::endl;
    game.PrintState(*state2);

    // Get dog actions
    std::vector<Action> dog_actions = game.GetActions(*state2);
    std::cout << "\nDogs have " << dog_actions.size() << " actions"
              << std::endl;

    if (dog_actions.empty()) {
        std::cerr << "ERROR: No dog actions!" << std::endl;
        return 1;
    }

    // Play several moves to create space
    std::unique_ptr<State> current_state = std::move(state2);
    for (int i = 0; i < 10; i++) {
        std::vector<Action> avail_actions = game.GetActions(*current_state);
        if (avail_actions.empty()) break;

        // Check for capture actions
        bool found_capture = false;
        for (const auto& action : avail_actions) {
            if (!game.IsNeighbor(action.cell_index_origin,
                                 action.cell_index_destination)) {
                std::cout << "\n=== CAPTURE FOUND at move " << i
                          << " ===" << std::endl;
                std::cout << "Move: " << action.cell_index_origin << " -> "
                          << action.cell_index_destination << std::endl;
                game.PrintState(*current_state);
                found_capture = true;
                break;
            }
        }

        if (found_capture) break;

        current_state = game.GetResult(*current_state, avail_actions[0]);
    }

    return 0;
}
