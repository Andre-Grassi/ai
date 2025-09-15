#include <queue>
#include <set>
#include <vector>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.7, page 91, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action>
using NodePtrVector = std::vector<std::shared_ptr<Node<State, Action>>>;

template <typename State, typename Action, typename NodeComparator>
std::shared_ptr<Node<State, Action>> search_algorithm::BestFirstSearch(
    Problem<State, Action> const& problem) {
    State initialState = problem.GetInitialState();
    std::shared_ptr<Node<State, Action>> root =
        std::make_shared<Node<State, Action>>(initialState);

    std::priority_queue<std::shared_ptr<Node<State, Action>>,
                        std::vector<std::shared_ptr<Node<State, Action>>>,
                        NodeComparator>
        frontier =
            std::priority_queue<std::shared_ptr<Node<State, Action>>,
                                NodePtrVector<State, Action>, NodeComparator>();
    frontier.push(root);

    std::set<State> reached = std::set<State>();
    reached.insert(root->GetState());

    // Increase depth limit until solution is found
    while (!frontier.empty()) {
        std::shared_ptr<Node<State, Action>> node = frontier.top();
        frontier.pop();

        if (problem.IsGoal(node->GetState())) return node;

        NodePtrVector<State, Action> children =
            node->Expand(const_cast<Problem<State, Action>&>(problem));
        for (const auto& child : children) {
            if (reached.find(child->GetState()) == reached.end()) {
                reached.insert(child->GetState());
                frontier.push(child);
            }
        }
    }

    return nullptr;  // failure
}