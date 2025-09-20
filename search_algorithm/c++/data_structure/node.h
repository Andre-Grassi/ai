/**
 * @file node.h
 * @brief Node class for search algorithms
 * @author Andre Grassi
 * @date 2025
 */

#ifndef SEARCH_ALG_DATA_STRUCTURE_NODE_H_
#define SEARCH_ALG_DATA_STRUCTURE_NODE_H_

#include <memory>

#include "problem.h"

/**
 * @brief Represents a node in a search tree
 *
 * A node contains a state, a pointer to its parent node, the action that
 * led to this state, and the cumulative path cost from the initial state.
 * Nodes are used to build search trees in various AI search algorithms.
 *
 * @tparam TState Type representing the problem state
 * @tparam TAction Type representing actions that can be taken
 * @tparam CostType Type representing the cost of actions (default is float)
 */
template <typename TState, typename TAction, typename CostType = float>
class Node
    : public std::enable_shared_from_this<Node<TState, TAction, CostType>> {
   public:
    /**
     * @brief Type alias for this node type
     */
    using NodeType = Node<TState, TAction, CostType>;

    explicit Node(TState state, std::shared_ptr<NodeType> parent = nullptr,
                  TAction action = TAction{}, CostType path_cost = 0.0)
        : state_(std::move(state)),
          parent_(std::move(parent)),
          action_(action),
          path_cost_(path_cost) {
        if (parent_)
            depth_ = parent_->GetDepth() + 1;
        else
            depth_ = 0;
    }

    /**
     * @brief Expands this node by generating all possible child nodes
     *
     * Creates child nodes for all valid actions from the current state.
     * Each child node has this node as its parent and the corresponding
     * action and updated path cost.
     *
     * @param problem The problem instance containing action and transition
     * logic
     * @return Vector of shared pointers to child nodes
     */
    std::vector <
        std::shared_ptr<NodeType> Expand(Problem<TState, TAction>& problem);

    /**
     * @brief Checks if this node creates a cycle in the current path
     *
     * Traverses up the parent chain to detect if the current state
     * has already appeared in the path from root to this node.
     *
     * @return true if a cycle is detected, false otherwise
     */
    bool IsCycle() const;

    /**
     * @brief Gets the state of this node
     * @return Const reference to the state
     */
    const TState& GetState() const { return state_; }

    /**
     * @brief Gets the parent node
     * @return Shared pointer to the parent node (nullptr for root)
     */
    std::shared_ptr<NodeType> GetParent() const { return parent_; }

    /**
     * @brief Gets the action that led to this node
     * @return Const reference to the action
     */
    const TAction& GetAction() const { return action_; }

    /**
     * @brief Gets the cumulative path cost from root to this node
     * @return The path cost as a float
     */
    float GetPathCost() const { return path_cost_; }

    /**
     * @brief Gets the depth of this node in the search tree
     * @return The depth as a uint64_t
     */
    uint64_t GetDepth() const { return depth_; }

   private:
    TState state_;
    std::shared_ptr<NodeType> parent_;
    TAction action_;
    float path_cost_;
    uint64_t depth_;

    /**
     * @name Private Setter Methods
     * @warning These methods are dangerous and should NOT be used after
     * construction. They can break node immutability and corrupt the search
     * tree structure. All node properties should be set only during
     * construction. Unless you are doing something really fancy.
     * @{
     */
    void SetState(const TState& state) { state_ = state; }
    void SetState(TState&& state) { state_ = std::move(state); }
    void SetParent(std::shared_ptr<NodeType> parent) {
        parent_ = std::move(parent);
    }
    void SetAction(const TAction& action) { action_ = action; }
    void SetAction(TAction&& action) { action_ = std::move(action); }
    void SetPathCost(CostType path_cost) { path_cost_ = path_cost; }
    void SetDepth(uint64_t depth) { depth_ = depth; }
    /// @}
};

#include "node.tpp"

#endif  // SEARCH_ALG_DATA_STRUCTURE_NODE_H_