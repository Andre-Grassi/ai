#ifndef __NODE_H__
#define __NODE_H__

#include <memory>

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
                  TAction action = TAction{}, double path_cost = 0.0)
        : state_(std::move(state)),
          parent_(std::move(parent)),
          action_(action),
          path_cost_(path_cost) {}

    const TState& get_state() const { return state_; }
    void set_state(const TState& state) { state_ = state; }
    void set_state(TState&& state) { state_ = std::move(state); }

    std::shared_ptr<Node<TState, TAction>> get_parent() const {
        return parent_;
    }
    void set_parent(std::shared_ptr<Node<TState, TAction>> parent) {
        parent_ = std::move(parent);
    }

    const TAction& get_action() const { return action_; }
    void set_action(const TAction& action) { action_ = action; }
    void set_action(TAction&& action) { action_ = std::move(action); }

    float get_path_cost() const { return path_cost_; }
    void set_path_cost(float path_cost) { path_cost_ = path_cost; }
};
#endif  // __NODE_H__