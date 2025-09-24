#include <ncurses.h>

#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include "../node.h"
#include "visual_node.h"

template <typename TState, typename TAction, typename CostType>
std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>>
VisualNode<TState, TAction, CostType>::Expand(
    Problem<TState, TAction, CostType>& problem) {
    // Call base class Expand to get child nodes
    auto base_children = Node<TState, TAction, CostType>::Expand(problem);

    // Convert base class nodes to VisualNode
    std::vector<std::shared_ptr<VisualNode<TState, TAction, CostType>>>
        children;
    for (size_t i = 0; i < base_children.size(); ++i) {
        std::string index_str = this->index_string_ + '.' + std::to_string(i);
        children.push_back(
            std::make_shared<VisualNode<TState, TAction, CostType>>(
                base_children[i], index_str));
    }

    this->children_ = std::move(children);

    return this->children_;
}

template <typename TState, typename TAction, typename CostType>
std::string VisualNode<TState, TAction, CostType>::GetTreeString() const {
    std::stringstream tree_ss;

    std::queue<std::shared_ptr<NodeType>> node_queue;
    node_queue.push(
        std::make_shared<VisualNode<TState, TAction, CostType>>(*this));

    uint64_t depth = this->GetDepth();

    while (!node_queue.empty()) {
        uint64_t this_depth_node_count = node_queue.size();

        // Get depth level
        tree_ss << std::to_string(depth) << ": ";

        // Process all nodes at the current depth
        for (uint64_t i = 0; i < this_depth_node_count; ++i) {
            std::shared_ptr<NodeType> current_node = node_queue.front();
            node_queue.pop();
            tree_ss << current_node->index_string_ << " ";

            // Enqueue children for the next depth level
            for (const auto& child : current_node->children_)
                node_queue.push(child);
        }
        tree_ss << std::endl;

        depth++;
    }

    return tree_ss.str();
}

template <typename TState, typename TAction, typename CostType>
void VisualNode<TState, TAction, CostType>::PrintTree() const {
    std::queue<std::shared_ptr<NodeType>> node_queue;
    node_queue.push(
        std::make_shared<VisualNode<TState, TAction, CostType>>(*this));

    uint64_t depth = this->GetDepth();

    while (!node_queue.empty()) {
        uint64_t this_depth_node_count = node_queue.size();

        // Print depth level
        std::cout << std::to_string(depth) << ": ";

        // Process all nodes at the current depth
        for (uint64_t i = 0; i < this_depth_node_count; ++i) {
            std::shared_ptr<NodeType> current_node = node_queue.front();
            node_queue.pop();
            std::cout << current_node->index_string_ << " ";

            // Enqueue children for the next depth level
            for (const auto& child : current_node->children_)
                node_queue.push(child);
        }
        std::cout << std::endl;

        depth++;
    }
}

template <typename TState, typename TAction, typename CostType>
std::string VisualNode<TState, TAction, CostType>::GetFrontierStatesString(
    const Problem<TState, TAction, CostType>& problem) const {
    std::stringstream frontier_states_ss;

    std::queue<std::shared_ptr<NodeType>> node_queue;
    node_queue.push(
        std::make_shared<VisualNode<TState, TAction, CostType>>(*this));

    std::queue<std::shared_ptr<NodeType>> frontier;

    while (!node_queue.empty()) {
        std::shared_ptr<NodeType> current_node = node_queue.front();
        node_queue.pop();

        if (current_node->children_.empty()) {
            frontier.push(current_node);
        } else {
            for (const auto& child : current_node->children_)
                node_queue.push(child);
        }
    }

    // Format the frontier states into a string
    while (!frontier.empty()) {
        std::shared_ptr<NodeType> node = frontier.front();
        frontier.pop();
        frontier_states_ss << node->index_string_ << ":\n"
                           << problem.GetStateString(node->GetState()) << "\n";
    }

    return frontier_states_ss.str();
}