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

    // Get the problem preset from cin
    int preset;
    std::cout << "Enter problem preset (1 or 2): " << std::endl;
    std::cin >> preset;

    // Create a Chess Board problem -- Parameter: 1 or 2 to set the
    // initial board as in the assignment statement
    chess_board::ChessBoardProblem problem(preset);

    // Get initial state
    chess_board::State initial_state = problem.GetInitialState();

    // Display initial state
    problem.PrintState(initial_state);

    std::shared_ptr<ChessNode> solution = search_algorithm::BestFirstSearch<
        chess_board::State, chess_board::Action, chess_board::ChessCostType,
        Comparator>(problem);

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

        problem.PrintAction(top_action);
        std::cout << std::endl;
    }

    std::cout << std::endl;
    problem.PrintState(solution->GetState());

    return 0;
}
