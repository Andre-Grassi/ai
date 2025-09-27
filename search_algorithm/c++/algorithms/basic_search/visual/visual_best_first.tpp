#include <queue>
#include <set>
#include <vector>

#include "data_structure/basic_search/problem.h"
#include "data_structure/visual/terminal_ui.h"
#include "data_structure/visual/visual_node.h"
#include "visual_search.h"

using namespace visual_search;

// Reference: figure 3.7, page 91, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action, typename CostType,
          typename Comparator>
std::shared_ptr<VisualNode<State, Action, CostType>>
visual_search::VisualBestFirstSearch(
    Problem<State, Action, CostType> const& problem) {
    using NodeType = VisualNode<State, Action, CostType>;
    using NodePtrVector = std::vector<std::shared_ptr<NodeType>>;

    int columns_layout = 2;
    TerminalUI ui(columns_layout);
    int left_window_index = 0;
    int right_window_index = 1;

    State initialState = problem.GetInitialState();
    std::string rootIndexStr = "0";
    std::shared_ptr<NodeType> root =
        std::make_shared<NodeType>(rootIndexStr, initialState);

    // Create concrete comparator instance
    Comparator comparator(problem);

    std::priority_queue<
        // Type of elements in the priority queue
        std::shared_ptr<NodeType>,
        // Container type for the priority queue
        NodePtrVector,
        // Comparator for the priority queue
        Comparator>
        frontier(comparator);

    frontier.push(root);

    std::set<State> reached = std::set<State>();
    reached.insert(root->GetState());

    // Increase depth limit until solution is found
    while (!frontier.empty()) {
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

        ui.WaitForKey('\n');

        std::shared_ptr<NodeType> node = frontier.top();
        frontier.pop();

        if (problem.IsGoal(node->GetState())) return node;

        NodePtrVector children = node->Expand(
            const_cast<Problem<State, Action, CostType>&>(problem));
        for (const auto& child : children) {
            if (reached.find(child->GetState()) == reached.end()) {
                reached.insert(child->GetState());
                frontier.push(child);
            }
        }
    }

    return nullptr;  // failure
}