#include <iostream>
#include <memory>

#include "algorithms/search_algorithm.h"
#include "data_structure/node.h"
#include "data_structure/node_comparator.h"
#include "data_structure/problems/sliding_tile_problem.h"

int main() {
    using NodeType =
        Node<sliding_tile::State, sliding_tile::Action, sliding_tile::CostType>;
    using TState = sliding_tile::State;
    using TAction = sliding_tile::Action;
    using TCost = sliding_tile::CostType;

    std::cout << "Search Algorithm Demo" << std::endl;

    uint64_t dimension;
    std::cout << "Enter board dimension (e.g., 3 for 3x3): ";
    std::cin >> dimension;

    // Create a sliding tile problem
    auto problem =
        std::make_unique<sliding_tile::SlidingTileProblem>(dimension);

    // Get initial state
    auto initial_state = problem->GetInitialState();

    std::cout << "Initial state created successfully!" << std::endl;
    std::cout << "Dimension: " << dimension << "x" << dimension << std::endl;

    // Display initial state
    problem->PrintState(initial_state);

    // Use A* comparator
    CompareByAStar<TState, TAction, TCost> comparator =
        CompareByAStar<TState, TAction, TCost>(*problem);

    std::shared_ptr<NodeType> solution =
        search_algorithm::BestFirstSearch<TState, TAction, TCost>(*problem,
                                                                  comparator);

    problem->PrintState(solution->GetState());

    return 0;
}
