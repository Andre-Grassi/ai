#ifndef NODE_COMPARATORS_H
#define NODE_COMPARATORS_H

#include <functional>
#include <memory>

#include "node.h"

namespace node_comparators {
// Min-heap (smallest cost first) based on path cost
struct CompareByPathCost {
    template <typename TState, typename TAction>
    bool operator()(const std::shared_ptr<Node<TState, TAction>>& a,
                    const std::shared_ptr<Node<TState, TAction>>& b) const {
        return a->GetPathCost() > b->GetPathCost();
    }
};
}  // namespace node_comparators
#endif  // NODE_COMPARATORS_H_