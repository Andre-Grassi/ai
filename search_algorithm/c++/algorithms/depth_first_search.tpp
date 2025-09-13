#include <set>
#include <stack>
#include <vector>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.9, page 96, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> search_algorithm::DepthFirstSearch(
    Problem<State, Action> const& problem) {
    auto root =
        std::make_shared<Node<State, Action>>(problem.GetInitialState());

    if (problem.IsGoal(root->GetState())) return root;

    std::stack<std::shared_ptr<Node<State, Action>>> lifo_stack =
        std::stack<std::shared_ptr<Node<State, Action>>>();
    lifo_stack.push(root);

    while (!lifo_stack.empty()) {
        std::shared_ptr<Node<State, Action>> node = lifo_stack.top();
        lifo_stack.pop();

        std::vector<std::shared_ptr<Node<State, Action>>> children =
            node->Expand(const_cast<Problem<State, Action>&>(problem));
        for (const auto& child : children) {
            if (problem.IsGoal(child->GetState())) return child;
            lifo_stack.push(child);
        }
    }

    return nullptr;  // Failure
}