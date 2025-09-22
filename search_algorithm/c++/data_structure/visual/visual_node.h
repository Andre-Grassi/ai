#ifndef SEARCH_ALG_DATA_STRUCTURE_VISUAL_VISUAL_NODE_H_
#define SEARCH_ALG_DATA_STRUCTURE_VISUAL_VISUAL_NODE_H_

#include <cstdint>
#include <string>
#include <vector>

#include "../node.h"

template <typename TState, typename TAction, typename CostType>
class VisualNode : public Node<TState, TAction, CostType> {
   public:
    using NodeType = VisualNode<TState, TAction, CostType>;
    using BaseType = Node<TState, TAction, CostType>;

    explicit VisualNode(std::string index_string, TState state,
                        std::shared_ptr<NodeType> parent = nullptr,
                        TAction action = TAction{}, CostType path_cost = 0.0)
        : BaseType(state, parent, action,
                   path_cost),  // This is like super()
          index_string_(index_string),
          children_() {}

    explicit VisualNode(std::shared_ptr<BaseType> node,
                        std::string index_string)
        : BaseType(node->GetState(), node->GetParent(), node->GetAction(),
                   node->GetPathCost()),
          index_string_(index_string),
          children_() {}

    std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>> Expand(
        Problem<TState, TAction, CostType>& problem);

    const std::string& GetIndexString() const { return index_string_; }

    // Can only be called after node is expanded
    const std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>>&
    GetChildren() const {
        return children_;
    }

    void PrintTree() const;

   private:
    std::string index_string_;
    std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>>
        children_;
};

#include "visual_node.tpp"

#endif  // SEARCH_ALG_DATA_STRUCTURE_VISUAL_VISUAL_NODE_H_
