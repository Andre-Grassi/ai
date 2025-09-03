#include <memory>

template <typename TState, typename TAction>
class Node {
   private:
    TState state_;
    std::shared_ptr<Node<TState, TAction>> parent_;
    TAction action_;
    float path_cost_;

   public:
    explicit Node(StateType state,
                  std::shared_ptr<Node<StateType, ActionType>> parent = nullptr,
                  ActionType action = ActionType{}, double path_cost = 0.0)
        : state_(std::move(state)),
          parent_(std::move(parent)),
          action_(action),
          path_cost_(path_cost) {}
};