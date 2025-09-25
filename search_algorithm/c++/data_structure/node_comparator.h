/**
 * @file node_comparator.h
 * @brief Comparator classes for ordering nodes in search algorithms
 * @author Andre Grassi
 * @date 2025
 */

#ifndef NODE_COMPARATOR_H
#define NODE_COMPARATOR_H

/**
 * @brief Base class for node comparators
 *
 * This class defines the interface for comparing nodes in search algorithms.
 * @tparam TState Type representing the problem state
 * @tparam TAction Type representing actions that can be taken
 * @tparam TCostType Type used for path costs (typically int, float, or double)
 */
template <typename TState, typename TAction, typename TCostType>
class NodeComparator {
   public:
    explicit NodeComparator(Problem<TState, TAction, TCostType> const& problem)
        : problem_(problem) {}

    virtual ~NodeComparator() = default;

    /**
     * @brief Comparison operator
     * @param lhs Left-hand side node
     * @param rhs Right-hand side node
     * @return true if lhs has lower priority than rhs
     */
    virtual bool operator()(
        std::shared_ptr<Node<TState, TAction, TCostType>> const& lhs,
        std::shared_ptr<Node<TState, TAction, TCostType>> const& rhs) const = 0;

   protected:
    Problem<TState, TAction, TCostType> const&
        problem_;  ///< Problem reference to access problem details, like
                   ///< heuristic function, if needed
};

/**
 * @brief Comparator that orders nodes by their path cost
 *
 * Orders nodes by path cost in ascending order, creating a min-heap where
 * nodes with lower path costs have higher priority. This is used by uniform
 * cost search to always expand the lowest-cost frontier node first.
 */
template <typename TState, typename TAction, typename TCostType>
class CompareByPathCost : public NodeComparator<TState, TAction, TCostType> {
   public:
    // Empty constructor, no attributes used in this comparator
    explicit CompareByPathCost() {}

    /**
     * @brief Compares two nodes by their path cost
     * @param lhs Left-hand side node
     * @param rhs Right-hand side node
     * @return true if lhs has higher path cost than rhs (lower priority)
     */
    bool operator()(
        std::shared_ptr<Node<TState, TAction, TCostType>> const& lhs,
        std::shared_ptr<Node<TState, TAction, TCostType>> const& rhs)
        const override {
        return lhs->GetPathCost() > rhs->GetPathCost();
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
 */
template <typename TState, typename TAction, typename TCostType>
class CompareByAStar : public NodeComparator<TState, TAction, TCostType> {
   public:
    /**
     * @brief Constructs the A* comparator with a problem instance
     *
     * @param problem The problem instance containing the heuristic function
     */
    explicit CompareByAStar(Problem<TState, TAction, TCostType> const& problem)
        : NodeComparator<TState, TAction, TCostType>(problem) {}

    /**
     * @brief Compares two nodes by their A* evaluation function f(n) = g(n) +
     * h(n)
     *
     * @param lhs First node to compare
     * @param rhs Second node to compare
     * @return true if node 'lhs' has higher f-value than 'rhs' (lower priority)
     *
     * @note The comparison returns true when 'lhs' should have lower priority
     * than 'rhs' in a priority queue, ensuring the min-heap property for A*
     * search
     */
    bool operator()(
        std::shared_ptr<Node<TState, TAction, TCostType>> const& lhs,
        std::shared_ptr<Node<TState, TAction, TCostType>> const& rhs)
        const override {
        TCostType g_lhs = lhs->GetPathCost();
        TCostType h_lhs = this->problem_.Heuristic(lhs->GetState());
        TCostType g_rhs = rhs->GetPathCost();
        TCostType h_rhs = this->problem_.Heuristic(rhs->GetState());
        return (g_lhs + h_lhs) > (g_rhs + h_rhs);
    }
};

#endif  // NODE_COMPARATOR_H
