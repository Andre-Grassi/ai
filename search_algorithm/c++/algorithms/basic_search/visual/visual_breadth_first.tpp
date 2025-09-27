#include <limits>
#include <queue>
#include <set>
#include <vector>

#include "data_structure/basic_search/problem.h"
#include "data_structure/node.h"
#include "data_structure/visual/terminal_ui.h"
#include "visual_search.h"

using namespace visual_search;

// Reference: figure 3.9, page 95, Artificial Intelligence: A Modern Approach,
// 4th edition

// Uses a set to avoid redundant paths
template <typename State, typename Action, typename CostType>
std::shared_ptr<VisualNode<State, Action, CostType>>
visual_search::VisualBreadthFirstSearch(
    Problem<State, Action, CostType> const& problem) {
    using NodeType = VisualNode<State, Action, CostType>;

    int columns_layout = 2;
    TerminalUI ui = TerminalUI(columns_layout);
    int left_window_index = 0;
    int right_window_index = 1;

    State initialState = problem.GetInitialState();
    std::shared_ptr<NodeType> root =
        std::make_shared<NodeType>("0", initialState);

    if (problem.IsGoal(root->GetState())) return root;

    std::queue<std::shared_ptr<NodeType>> fifo_queue =
        std::queue<std::shared_ptr<NodeType>>();
    fifo_queue.push(root);

    std::set<State> reached = std::set<State>();
    reached.insert(root->GetState());
    while (!fifo_queue.empty()) {
        /*
         * Print Tree
         */
        ui.RefreshAll();
        ui.PrintToWindow(left_window_index, 0, 0,
                         "Tree:\n" + root->GetTreeString());
        ui.PrintToWindow(
            right_window_index, 0, 0,
            "Frontier States:\n" + root->GetFrontierStatesString(problem));
        ui.PrintToStatusBar("Press Enter to continue...");
        ui.RefreshAll();

        // Wait for user input to proceed
        // std::cout << "Press Enter to continue..." << std::endl;
        ui.WaitForKey('\n');

        std::shared_ptr<NodeType> node = fifo_queue.front();
        fifo_queue.pop();

        std::vector<std::shared_ptr<NodeType>> children = node->Expand(
            const_cast<Problem<State, Action, CostType>&>(problem));

        for (const auto& child : children) {
            if (problem.IsGoal(child->GetState())) return child;
            if (reached.find(child->GetState()) == reached.end()) {
                reached.insert(child->GetState());
                fifo_queue.push(child);
            }
        }
    }

    return nullptr;  // Failure
}