#include <set>
#include <stack>
#include <vector>

#include "data_structure/basic_search/problem.h"
#include "data_structure/node.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: page 96, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>>
search_algorithm::DepthFirstSearch(
    Problem<State, Action, CostType> const& problem) {
    using NodeType = Node<State, Action, CostType>;

    auto root = std::make_shared<NodeType>(problem.GetInitialState());

    if (problem.IsGoal(root->GetState())) return root;

    std::stack<std::shared_ptr<NodeType>> lifo_stack =
        std::stack<std::shared_ptr<NodeType>>();
    lifo_stack.push(root);

    while (!lifo_stack.empty()) {
        std::shared_ptr<NodeType> node = lifo_stack.top();
        lifo_stack.pop();

        std::vector<std::shared_ptr<NodeType>> children = node->Expand(
            const_cast<Problem<State, Action, CostType>&>(problem));
        for (const auto& child : children) {
            // DEBUG not sure if this is the correct place for goal test, but
            // makes sense
            if (problem.IsGoal(child->GetState())) return child;
            lifo_stack.push(child);
        }
    }

    return nullptr;  // Failure
}