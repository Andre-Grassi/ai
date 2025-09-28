#include <iostream>
#include <memory>
#include <stack>

#include "algorithms/search_algorithm.h"
#include "data_structure/node.h"
#include "data_structure/node_comparator.h"
#include "data_structure/problems/chess_board_problem.h"

int main() {
    // Use A* comparator
    using Comparator = CompareByAStar<chess_board::State, chess_board::Action,
                                      chess_board::ChessCostType>;

    using ChessNode = Node<chess_board::State, chess_board::Action,
                           chess_board::ChessCostType>;

    // Create a Chess Board problem -- Parametro: 1 ou 2 pra definir o
    // tabuleiro inicial igual no enunciado
    auto problem = std::make_unique<chess_board::ChessBoardProblem>(1);

    // Get initial state
    auto initial_state = problem->GetInitialState();

    // Display initial state
    problem->PrintState(initial_state);

    std::shared_ptr<ChessNode> solution = search_algorithm::BestFirstSearch<
        chess_board::State, chess_board::Action, chess_board::ChessCostType,
        Comparator>(*problem);

    if (!solution) {
        std::cout << "Goal State Not Found." << std::endl;
        return 1;
    }

    // Stack the actions from the solution (leaf) node to the root and then
    // print them
    std::stack<chess_board::Action> action_stack;

    // Collect actions from leaf -> root, skipping root (it has no action)
    for (std::shared_ptr<ChessNode> node = solution; node && node->GetParent();
         node = node->GetParent())
        action_stack.push(node->GetAction());

    // Print Actions
    while (!action_stack.empty()) {
        chess_board::Action &top_action = action_stack.top();
        action_stack.pop();

        problem->PrintAction(top_action);
        std::cout << std::endl;
    }

    std::cout << std::endl;
    problem->PrintState(solution->GetState());

    return 0;
}
