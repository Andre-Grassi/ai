#ifndef __NODE_H__
#define __NODE_H__

#include <memory>

#include "problem.h"

template <typename TState, typename TAction>
class Node : public std::enable_shared_from_this<Node<TState, TAction>> {
   private:
    TState state_;
    std::shared_ptr<Node<TState, TAction>> parent_;
    TAction action_;
    float path_cost_;
    uint64_t depth_;

   public:
    explicit Node(TState state,
                  std::shared_ptr<Node<TState, TAction>> parent = nullptr,
                  TAction action = TAction{}, float path_cost = 0.0)
        : state_(std::move(state)),
          parent_(std::move(parent)),
          action_(action),
          path_cost_(path_cost) {
        if (parent_)
            depth_ = parent_->GetDepth() + 1;
        else
            depth_ = 0;
    }

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

    uint64_t GetDepth() const { return depth_; }
};

#include "node.tpp"

#endif  // __NODE_H__