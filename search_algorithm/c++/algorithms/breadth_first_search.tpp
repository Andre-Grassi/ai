#include <queue>
#include <unordered_set>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.9, page 95, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action>
Node<State, Action>* search_algorithm::BreadthFirstSearch(
    Problem<State, Action> const& problem) {
    Node root = Node(problem.GetInitialState());

    if (problem.IsGoal(root.GetState())) return &root;

    std::queue<std::shared_ptr<Node<State, Action>>> fifo_queue =
        std::queue<std::shared_ptr<Node<State, Action>>>();
    fifo_queue.push(std::make_shared<Node<State, Action>>(root));

    std::unordered_set<State> reached = std::unordered_set<State>();
    reached.insert(root.GetState());

    while (!fifo_queue.empty()) {
        std::shared_ptr<Node<State, Action>> node = fifo_queue.front();
        fifo_queue.pop();

        std::vector<std::shared_ptr<Node<State, Action>>> children =
            node->Expand(const_cast<Problem<State, Action>&>(problem));
        for (const auto& child : children) {
            if (problem.IsGoal(child->GetState())) return child.get();
            if (reached.find(child->GetState()) == reached.end()) {
                reached.insert(child->GetState());
                fifo_queue.push(child);
            }
        }
    }

    return nullptr;  // Failure
}