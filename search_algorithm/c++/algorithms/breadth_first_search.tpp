#include <queue>
#include <set>
#include <vector>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.9, page 95, Artificial Intelligence: A Modern Approach,
// 4th edition

// Uses a set to avoid redundant paths
template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> search_algorithm::BreadthFirstSearch(
    Problem<State, Action> const& problem) {
    State initialState = problem.GetInitialState();
    std::shared_ptr<Node<State, Action>> root =
        std::make_shared<Node<State, Action>>(initialState);

    if (problem.IsGoal(root->GetState())) return root;

    std::queue<std::shared_ptr<Node<State, Action>>> fifo_queue =
        std::queue<std::shared_ptr<Node<State, Action>>>();
    fifo_queue.push(root);

    std::set<State> reached = std::set<State>();
    reached.insert(root->GetState());

    while (!fifo_queue.empty()) {
        std::shared_ptr<Node<State, Action>> node = fifo_queue.front();
        fifo_queue.pop();

        std::vector<std::shared_ptr<Node<State, Action>>> children =
            node->Expand(const_cast<Problem<State, Action>&>(problem));
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