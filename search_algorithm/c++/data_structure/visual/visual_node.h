/**
 * @file visual_node.h
 * @brief Visual representation of search tree nodes for debugging and
 * visualization
 * @author Andre Grassi
 * @date 2025
 */

#ifndef SEARCH_ALG_DATA_STRUCTURE_VISUAL_VISUAL_NODE_H_
#define SEARCH_ALG_DATA_STRUCTURE_VISUAL_VISUAL_NODE_H_

#include <cstdint>
#include <queue>
#include <string>
#include <vector>

#include "data_structure/node.h"

/**
 * @brief A visual representation of a search tree node that extends the base
 * Node class
 *
 * VisualNode adds visualization capabilities to the standard search node,
 * including the ability to display the search tree structure and maintain
 * hierarchical index strings for easy identification of nodes in the tree.
 *
 * @tparam TState The type representing the state of the problem
 * @tparam TAction The type representing actions that can be taken
 * @tparam CostType The type used for path costs (typically int, float, or
 * double)
 */
template <typename TState, typename TAction, typename CostType>
class VisualNode : public Node<TState, TAction, CostType> {
   public:
    /// @brief Type alias for VisualNode with same template parameters
    using NodeType = VisualNode<TState, TAction, CostType>;
    /// @brief Type alias for the base Node class
    using BaseType = Node<TState, TAction, CostType>;

    /**
     * @brief Construct a new VisualNode object with full parameters
     *
     * @param index_string Hierarchical string identifier for this node (e.g.,
     * "0.1.2")
     * @param state The state this node represents
     * @param parent Shared pointer to the parent VisualNode (nullptr for root)
     * @param action The action taken to reach this state from parent
     * @param path_cost The cumulative cost from root to this node
     */
    explicit VisualNode(std::string index_string, TState state,
                        std::shared_ptr<NodeType> parent = nullptr,
                        TAction action = TAction{}, CostType path_cost = 0.0)
        : BaseType(state, parent, action,
                   path_cost),  // This is like super()
          index_string_(index_string),
          children_() {}

    /**
     * @brief Construct a VisualNode from an existing base Node
     *
     * @param node Shared pointer to an existing Node to convert
     * @param index_string Hierarchical string identifier for this visual node
     */
    explicit VisualNode(std::shared_ptr<BaseType> node,
                        std::string index_string)
        : BaseType(node->GetState(), node->GetParent(), node->GetAction(),
                   node->GetPathCost()),
          index_string_(index_string),
          children_() {}

    /**
     * @brief Expand this node by generating all possible successor states
     *
     * Creates child VisualNode objects for each action available from the
     * current state. Each child is assigned a hierarchical index string based
     * on its position.
     *
     * @param problem The problem instance providing actions and state
     * transitions
     * @return std::vector<std::shared_ptr<VisualNode<TState, TAction,
     * CostType>>> Vector of shared pointers to child VisualNode objects
     */
    std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>> Expand(
        Problem<TState, TAction, CostType>& problem);

    /**
     * @brief Get the hierarchical index string for this node
     *
     * @return const std::string& Reference to the index string (e.g., "0.1.2")
     */
    const std::string& GetIndexString() const { return index_string_; }

    /**
     * @brief Get the children of this node
     *
     * @warning If this method is called before Expand(), it will return an
     * empty vector.
     * @return const std::vector<std::shared_ptr<VisualNode<TState, TAction,
     * CostType>>>& Reference to the vector of child nodes
     */
    const std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>>&
    GetChildren() const {
        return children_;
    }

    std::string GetTreeString() const;

    /**
     * @brief Print the entire search tree starting from the root
     *
     * Displays the tree structure level by level, showing the hierarchical
     * index strings for each node.
     * Assumes that the node that called this method is the root.
     *
     * Output format:
     * - Depth 0: [root index]
     * - Depth 1: [child1 index] [child2 index] ...
     * - Depth 2: [grandchild indices] ...
     */
    void PrintTree() const;

    std::queue<std::shared_ptr<VisualNode<TState, TAction, CostType>>>
    GetFrontierStates(const Problem<TState, TAction, CostType>& problem) const;

    // Get the frontier states and format them as a string
    std::string GetFrontierStatesString(
        const Problem<TState, TAction, CostType>& problem) const;

    // Given the frontier states, format according to the problem
    std::string FormatFrontierStates(
        std::queue<std::shared_ptr<VisualNode<TState, TAction, CostType>>>
            frontier,
        const Problem<TState, TAction, CostType>& problem) const;

   private:
    /**
     * @brief Hierarchical index string for node identification
     *
     * Format: "x.y.z" where each number represents the position at each depth
     * level. Example: "0.1.2" means root's second child's third child.
     */
    std::string index_string_;

    /**
     * @brief Vector of child VisualNode pointers
     *
     * Populated when Expand() is called. Empty vector indicates either:
     * 1. Node has not been expanded yet, or
     * 2. Node is a leaf with no valid actions
     */
    std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>>
        children_;
};

// Include template implementation
#include "visual_node.tpp"

#endif  // SEARCH_ALG_DATA_STRUCTURE_VISUAL_VISUAL_NODE_H_
