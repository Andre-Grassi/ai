#include <set>
#include <stack>
#include <vector>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.12, page 99, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> search_algorithm::DepthLimitedSearch(
    Problem<State, Action> const& problem, uint64_t depth_limit,
    bool check_node_cycles) {
    auto root =
        std::make_shared<Node<State, Action>>(problem.GetInitialState());

    std::stack<std::shared_ptr<Node<State, Action>>> lifo_stack =
        std::stack<std::shared_ptr<Node<State, Action>>>();
    lifo_stack.push(root);

    while (!lifo_stack.empty()) {
        std::shared_ptr<Node<State, Action>> node = lifo_stack.top();
        lifo_stack.pop();

        if (problem.IsGoal(node->GetState())) return node;  // Solution found

        if (node->GetDepth() <= depth_limit) {
            if (check_node_cycles && node->IsCycle()) continue;

            std::vector<std::shared_ptr<Node<State, Action>>> children =
                node->Expand(const_cast<Problem<State, Action>&>(problem));
            for (const auto& child : children) lifo_stack.push(child);
        }
    }

    return nullptr;  // Failure or cutoff
}