#include <vector>

#include "node.h"
#include "problem.h"

template <typename TState, typename TAction, typename CostType>
std::vector<std::shared_ptr<Node<TState, TAction, CostType>>>
Node<TState, TAction, CostType>::Expand(
    Problem<TState, TAction, CostType>& problem) {
    using NodeType = Node<TState, TAction, CostType>;

    TState current_state = this->GetState();
    std::vector<TAction> actions = problem.GetActions(current_state);
    std::vector<std::shared_ptr<NodeType>> children;

    for (const TAction& action : actions) {
        std::unique_ptr<TState> new_state =
            problem.GetResult(current_state, action);
        float cost = this->GetPathCost() +
                     problem.GetActionCost(current_state, action, *new_state);
        children.emplace_back(std::make_shared<NodeType>(
            *new_state, this->shared_from_this(), action, cost));
    }

    return children;
}

template <typename TState, typename TAction, typename CostType>
bool Node<TState, TAction, CostType>::IsCycle() const {
    auto parent = this->GetParent();
    while (parent) {
        if (parent->GetState() == this->GetState()) return true;
        parent = parent->GetParent();
    }
    return false;
}
