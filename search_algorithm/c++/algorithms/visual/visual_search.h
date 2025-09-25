/**
 * @file visual_search.h
 * @brief Collection of AI search algorithms with visual debugging capabilities
 * @author Andre Grassi
 * @date 2025
 *
 * This header file provides visual implementations of classic AI search
 * algorithms that display the search tree expansion process step-by-step for
 * educational and debugging purposes.
 */

#ifndef SEARCH_ALG_ALGORITHMS_VISUAL_VISUAL_SEARCH_H_
#define SEARCH_ALG_ALGORITHMS_VISUAL_VISUAL_SEARCH_H_

#include "data_structure/node.h"
#include "data_structure/node_comparator.h"
#include "data_structure/problem.h"
#include "data_structure/problems/sliding_tile_problem.h"
#include "data_structure/visual/visual_node.h"

/**
 * @namespace visual_search
 * @brief Contains visual implementations of AI search algorithms
 *
 * This namespace provides search algorithms that include step-by-step
 * visualization capabilities for educational and debugging purposes. Each
 * algorithm displays the search tree expansion process, allowing users to
 * observe how the search space is explored.
 */
namespace visual_search {

/**
 * @brief Visual implementation of the Breadth-First Search algorithm
 *
 * This function implements the classic BFS algorithm with visual debugging
 * capabilities. It displays the search tree expansion process step-by-step,
 * showing each level of the tree as it's explored. The user can step through
 * the search process interactively.
 *
 * The algorithm follows the standard BFS approach:
 * 1. Start with the initial state in a FIFO queue
 * 2. For each node, expand all children
 * 3. Display the current tree structure
 * 4. Wait for user input before continuing
 * 5. Add children to the queue if they haven't been reached
 * 6. Continue until goal is found or queue is empty
 *
 * @tparam State The type representing the state of the problem
 * @tparam Action The type representing actions that can be taken
 * @tparam CostType The type used for path costs (typically int, float, or
 * double)
 *
 * @param problem The problem instance defining initial state, goal test, and
 * available actions
 *
 * @return std::shared_ptr<VisualNode<State, Action, CostType>>
 *         Shared pointer to the goal node if found, nullptr if no solution
 * exists
 *
 * @note This function uses a set to track reached states and avoid redundant
 * paths
 * @note The search process is interactive - user must press Enter to continue
 * between steps
 *
 * @see Node::Expand() for details on how child nodes are generated
 * @see VisualNode::PrintTree() for tree visualization format
 */
template <typename State, typename Action, typename CostType>
std::shared_ptr<VisualNode<State, Action, CostType>> VisualBreadthFirstSearch(
    Problem<State, Action, CostType> const& problem);

template <typename State, typename Action, typename CostType,
          typename Comparator>
std::shared_ptr<VisualNode<State, Action, CostType>> VisualBestFirstSearch(
    Problem<State, Action, CostType> const& problem);
}  // namespace visual_search

// Include template implementation
#include "visual_best_first.tpp"
#include "visual_breadth_first.tpp"
#endif  // SEARCH_ALG_ALGORITHMS_VISUAL_VISUAL_SEARCH_H_