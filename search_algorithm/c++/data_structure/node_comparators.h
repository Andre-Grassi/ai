/**
 * @file node_comparators.h
 * @brief Comparator functions for ordering nodes in search algorithms
 * @author Andre Grassi
 * @date 2025
 */

#ifndef NODE_COMPARATORS_H
#define NODE_COMPARATORS_H

#include <functional>
#include <memory>

#include "node.h"
#include "problem.h"

/**
 * @namespace node_comparators
 * @brief Contains comparator structs for different search algorithm strategies
 *
 * This namespace provides various comparison functions used by priority queues
 * in search algorithms. Each comparator implements a different node ordering
 * strategy corresponding to different search algorithms.
 */
namespace node_comparators {

/**
 * @brief Comparator that orders nodes by their path cost
 *
 * Orders nodes by path cost in ascending order, creating a min-heap where
 * nodes with lower path costs have higher priority. This is used by uniform
 * cost search to always expand the lowest-cost frontier node first.
 *
 * @note Returns true if 'a' should have lower priority than 'b' in a priority
 * queue
 */
struct CompareByPathCost {
    /**
     * @brief Compares two nodes by their path cost
     *
     * @tparam TState Type representing the problem state
     * @tparam TAction Type representing actions that can be taken
     * @param a First node to compare
     * @param b Second node to compare
     * @return true if node 'a' has higher path cost than 'b' (lower priority)
     */
    template <typename TState, typename TAction, typename CostType>
    bool operator()(
        const std::shared_ptr<Node<TState, TAction, CostType>>& a,
        const std::shared_ptr<Node<TState, TAction, CostType>>& b) const {
        return a->GetPathCost() > b->GetPathCost();
    }
};

/**
 * @brief Comparator for A* search algorithm
 *
 * Orders nodes by their evaluation function f(n) = g(n) + h(n), where:
 * - g(n) is the path cost from start to node n
 * - h(n) is the heuristic estimate from node n to goal
 *
 * Creates a min-heap where nodes with lower f-values have higher priority.
 * This ensures A* always expands the most promising node first.
 *
 * @note Need to provide a Problem instance in construction to access the
 * heuristic function
 *
 * @tparam TState Type representing the problem state
 * @tparam TAction Type representing actions that can be taken
 */
template <typename TState, typename TAction, typename CostType>
struct CompareByAStar {
   private:
    Problem<TState, TAction, CostType>
        problem_;  ///< Problem reference to access heuristic function

   public:
    /**
     * @brief Constructs the A* comparator with a problem instance
     *
     * @param problem The problem instance containing the heuristic function
     */
    CompareByAStar(Problem<TState, TAction, CostType> problem)
        : problem_(problem) {}

    /**
     * @brief Compares two nodes by their A* evaluation function f(n) = g(n) +
     * h(n)
     *
     * @param a First node to compare
     * @param b Second node to compare
     * @return true if node 'a' has higher f-value than 'b' (lower priority)
     *
     * @note The comparison returns true when 'a' should have lower priority
     * than 'b' in a priority queue, ensuring the min-heap property for A*
     * search
     */
    bool operator()(
        const std::shared_ptr<Node<TState, TAction, CostType>>& a,
        const std::shared_ptr<Node<TState, TAction, CostType>>& b) const {
        return (a->GetPathCost() + problem_.Heuristic(a->GetState())) >
               (b->GetPathCost() + problem_.Heuristic(b->GetState()));
    }
};

}  // namespace node_comparators
#endif  // NODE_COMPARATORS_H_