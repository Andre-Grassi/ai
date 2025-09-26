#include <iostream>
#include <memory>

#include "algorithms/basic_search/search_algorithm.h"
#include "data_structure/basic_search/problems/chess_board_problem.h"
#include "data_structure/node.h"
#include "data_structure/node_comparator.h"

int main() {
    // Use A* comparator
    using Comparator = CompareByAStar<chess_board::State, chess_board::Action,
                                      chess_board::ChessCostType>;

    // Create a Chess Board problem -- Parametro: 1 ou 2 pra definir o tabuleiro
    // inicial igual no enunciado
    auto problem = std::make_unique<chess_board::ChessBoardProblem>(2);

    // Get initial state
    auto initial_state = problem->GetInitialState();
    std::cout << "Initial State Created Successfully!" << std::endl;

    // Display initial state
    problem->PrintState(initial_state);

    std::shared_ptr<Node<chess_board::State, chess_board::Action,
                         chess_board::ChessCostType>>
        solution = search_algorithm::BestFirstSearch<
            chess_board::State, chess_board::Action, chess_board::ChessCostType,
            Comparator>(*problem);

    if (solution) {
        std::cout << "Goal State Found!" << std::endl;
        problem->PrintState(solution->GetState());
    } else
        std::cout << "Goal State Not Found." << std::endl;

    return 0;
}
