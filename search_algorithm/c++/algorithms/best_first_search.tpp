#include <queue>
#include <set>
#include <vector>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.7, page 91, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action, typename CostType>
using NodePtrVector = std::vector<std::shared_ptr<Node<State, Action, CostType>>>;

template <typename State, typename Action, typename CostType,
          typename NodeComparator>
std::shared_ptr<Node<State, Action, CostType>>
search_algorithm::BestFirstSearch(
    Problem<State, Action, CostType> const& problem) {
    State initialState = problem.GetInitialState();
    std::shared_ptr<Node<State, Action, CostType>> root =
        std::make_shared<Node<State, Action, CostType>>(initialState);

    std::priority_queue<
        std::shared_ptr<Node<State, Action, CostType>>,
        std::vector<std::shared_ptr<Node<State, Action, CostType>>>,
        NodeComparator>
        frontier =
            std::priority_queue<std::shared_ptr<Node<State, Action, CostType>>,
                                NodePtrVector<State, Action, CostType>,
                                NodeComparator>();
    frontier.push(root);

    std::set<State> reached = std::set<State>();
    reached.insert(root->GetState());

    // Increase depth limit until solution is found
    while (!frontier.empty()) {
        std::shared_ptr<Node<State, Action, CostType>> node = frontier.top();
        frontier.pop();

        if (problem.IsGoal(node->GetState())) return node;

        NodePtrVector<State, Action, CostType> children =
            node->Expand(const_cast<Problem<State, Action, CostType>&>(problem));
        for (const auto& child : children) {
            if (reached.find(child->GetState()) == reached.end()) {
                reached.insert(child->GetState());
                frontier.push(child);
            }
        }
    }

    return nullptr;  // failure
}