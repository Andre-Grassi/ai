#ifndef __NODE_H__
#define __NODE_H__

#include <memory>

#include "node.tpp"

template <typename TState, typename TAction>
class Node {
   private:
    TState state_;
    std::shared_ptr<Node<TState, TAction>> parent_;
    TAction action_;
    float path_cost_;

   public:
    explicit Node(TState state,
                  std::shared_ptr<Node<TState, TAction>> parent = nullptr,
                  TAction action = TAction{}, float path_cost = 0.0)
        : state_(std::move(state)),
          parent_(std::move(parent)),
          action_(action),
          path_cost_(path_cost) {}

    std::vector<std::shared_ptr<Node<TState, TAction>>> Expand(
        Problem<TState, TAction>& problem);

    const TState& GetState() const { return state_; }
    void SetState(const TState& state) { state_ = state; }
    void SetState(TState&& state) { state_ = std::move(state); }

    std::shared_ptr<Node<TState, TAction>> GetParent() const { return parent_; }
    void SetParent(std::shared_ptr<Node<TState, TAction>> parent) {
        parent_ = std::move(parent);
    }

    const TAction& GetAction() const { return action_; }
    void SetAction(const TAction& action) { action_ = action; }
    void SetAction(TAction&& action) { action_ = std::move(action); }

    float GetPathCost() const { return path_cost_; }
    void SetPathCost(float path_cost) { path_cost_ = path_cost; }
};
#endif  // __NODE_H__