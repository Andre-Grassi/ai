#ifndef NODE_COMPARATORS_H
#define NODE_COMPARATORS_H

#include <functional>
#include <memory>

#include "node.h"
#include "problem.h"

namespace node_comparators {
// Min-heap (smallest cost first) based on path cost
struct CompareByPathCost {
    template <typename TState, typename TAction>
    bool operator()(const std::shared_ptr<Node<TState, TAction>>& a,
                    const std::shared_ptr<Node<TState, TAction>>& b) const {
        return a->GetPathCost() > b->GetPathCost();
    }
};

template <typename TState, typename TAction>
struct CompareByAStar {
   private:
    // Problem reference to access heuristic function
    Problem<TState, TAction> problem_;

   public:
    CompareByAStar(Problem<TState, TAction> problem) : problem_(problem) {}

    bool operator()(const std::shared_ptr<Node<TState, TAction>>& a,
                    const std::shared_ptr<Node<TState, TAction>>& b) const {
        return (a->GetPathCost() + problem_.Heuristic(a->GetState())) >
               (b->GetPathCost() + problem_.Heuristic(b->GetState()));
    }
};
}  // namespace node_comparators
#endif  // NODE_COMPARATORS_H_