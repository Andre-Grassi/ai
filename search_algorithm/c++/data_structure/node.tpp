#include <vector>

#include "node.h"
#include "problem.h"

template <typename TState, typename TAction>
std::vector<std::shared_ptr<Node<TState, TAction>>>
Node<TState, TAction>::Expand(Problem<TState, TAction>& problem) {
    TState current_state = this->GetState();
    std::vector<TAction> actions = problem.GetActions(current_state);
    std::vector<std::shared_ptr<Node<TState, TAction>>> children;

    for (const TAction& action : actions) {
        std::unique_ptr<TState> new_state =
            problem.GetResult(current_state, action);
        float cost = this->GetPathCost() +
                     problem.GetActionCost(current_state, action, *new_state);
        children.emplace_back(std::make_shared<Node<TState, TAction>>(
            *new_state, this->shared_from_this(), action, cost));
    }

    return children;
}

template <typename TState, typename TAction>
bool Node<TState, TAction>::IsCycle() const {
    auto parent = this->GetParent();
    while (parent) {
        if (parent->GetState() == this->GetState()) return true;
        parent = parent->GetParent();
    }
    return false;
}
