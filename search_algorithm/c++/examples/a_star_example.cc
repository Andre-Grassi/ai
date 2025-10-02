#include <iostream>
#include <memory>

#include "algorithms/basic_search/search_algorithm.h"
#include "data_structure/basic_search/problems/sliding_tile_problem.h"
#include "data_structure/node.h"
#include "data_structure/node_comparator.h"

int main() {
    using NodeType =
        Node<sliding_tile::State, sliding_tile::Action, sliding_tile::CostType>;
    using TState = sliding_tile::State;
    using TAction = sliding_tile::Action;
    using TCost = sliding_tile::CostType;

    // Use A* comparator
    using Comparator = CompareByAStar<TState, TAction, TCost>;

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

    std::shared_ptr<NodeType> solution =
        search_algorithm::BestFirstSearch<TState, TAction, TCost, Comparator>(
            *problem);

    problem->PrintState(solution->GetState());

    return 0;
}
