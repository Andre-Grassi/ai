#include "node.h"

#include <vector>

#include "problem.h"

template <typename TState, typename TAction>
std::vector<std::shared_ptr<Node<TState, TAction>>>
Node<TState, TAction>::Expand(Problem<TState, TAction>& problem) {
    TState current_state = this->get_state();
    std::vector<TAction> actions = problem.GetActions(current_state);
    std::vector<std::shared_ptr<Node<TState, TAction>>> children;

    for (const TAction& action : actions) {
        TState new_state = problem.GetResult(current_state, action);
        float cost = this->get_path_cost() +
                     problem.GetActionCost(current_state, action, new_state);
        children.emplace_back(std::make_shared<Node<TState, TAction>>(
            new_state, shared_from_this(), action, cost));
    }

    return children;
}
