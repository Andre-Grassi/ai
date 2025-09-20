#include <set>
#include <stack>
#include <vector>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.12, page 99, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>>
search_algorithm::DepthLimitedSearch(
    Problem<State, Action, CostType> const& problem, uint64_t depth_limit,
    bool check_node_cycles, bool* out_cutoff) {
    using NodeType = Node<State, Action, CostType>;

    auto root = std::make_shared<NodeType>(problem.GetInitialState());

    std::stack<std::shared_ptr<NodeType>> lifo_stack =
        std::stack<std::shared_ptr<NodeType>>();
    lifo_stack.push(root);

    bool cutoff_occurred = false;

    while (!lifo_stack.empty()) {
        std::shared_ptr<NodeType> node = lifo_stack.top();
        lifo_stack.pop();

        if (problem.IsGoal(node->GetState())) return node;  // Solution found

        if (node->GetDepth() <= depth_limit) {
            if (check_node_cycles && node->IsCycle()) continue;

            std::vector<std::shared_ptr<NodeType>> children = node->Expand(
                const_cast<Problem<State, Action, CostType>&>(problem));
            for (const auto& child : children) lifo_stack.push(child);
        } else
            cutoff_occurred = true;
    }

    if (cutoff_occurred) *out_cutoff = true;

    return nullptr;  // Failure or cutoff (cutoff is indicated via out_cutoff)
}