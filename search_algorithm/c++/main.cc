#include <iostream>
#include <memory>

#include "algorithms/search_algorithm.h"
#include "data_structure/node.h"
#include "data_structure/problems/sliding_tile_problem.h"

int main() {
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

    std::shared_ptr<Node<sliding_tile::State, sliding_tile::Action>> solution =
        search_algorithm::BreadthFirstSearch(*problem);

    problem->PrintState(solution->GetState());

    return 0;
}
