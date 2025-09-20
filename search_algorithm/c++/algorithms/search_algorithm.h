/**
 * @file search_algorithm.h
 * @brief Collection of AI search algorithm implementations
 * @author Andre Grassi
 * @date 2025
 */

#ifndef SEARCH_ALG_ALGORITHMS_SEARCH_ALGORITHM_H_
#define SEARCH_ALG_ALGORITHMS_SEARCH_ALGORITHM_H_

#include "../data_structure/node.h"
#include "../data_structure/node_comparators.h"
#include "../data_structure/problem.h"
#include "../data_structure/problems/sliding_tile_problem.h"

/**
 * @namespace search_algorithm
 * @brief Contains implementations of various AI search algorithms
 * 
 * This namespace provides a comprehensive collection of search algorithms
 * for solving AI problems. All algorithms work with the generic Problem
 * interface and return solution paths as Node trees.
 * 
 * Algorithms include:
 * - Uninformed search: BFS, DFS, DLS, IDS, UCS
 * - Informed search: Best-first search (can be used for A*, Greedy, etc.)
 */
namespace search_algorithm {

/**
 * @brief Breadth-First Search algorithm
 * 
 * @tparam State Type representing problem states
 * @tparam Action Type representing actions/moves
 * @tparam CostType Type for action costs (default: float)
 * @param problem The problem instance to solve
 * @return Shared pointer to goal node, or nullptr if no solution exists
 */
template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>> BreadthFirstSearch(
    Problem<State, Action, CostType> const& problem);

/**
 * @brief Depth-First Search algorithm
 * 
 * @tparam State Type representing problem states
 * @tparam Action Type representing actions/moves
 * @tparam CostType Type for action costs
 * @param problem The problem instance to solve
 * @return Shared pointer to goal node, or nullptr if no solution exists
 */
template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>> DepthFirstSearch(
    Problem<State, Action, CostType> const& problem);

/**
 * @brief Depth-Limited Search algorithm
 * 
 * @tparam State Type representing problem states
 * @tparam Action Type representing actions/moves
 * @tparam CostType Type for action costs
 * @param problem The problem instance to solve
 * @param depth_limit Maximum depth to search
 * @param check_node_cycles If true, check for cycles (more expensive but avoids redundant paths)
 * @param out_cutoff Output parameter: set to true if cutoff occurred, false if no solution exists
 * @return Shared pointer to goal node, or nullptr if no solution within depth limit
 */
template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>> DepthLimitedSearch(
    Problem<State, Action, CostType> const& problem, uint64_t depth_limit,
    bool check_node_cycles, bool* out_cutoff);

/**
 * @brief Iterative Deepening Search algorithm
 * 
 * Calls Depth-Limited Search with increasing depth limits until a solution
 * is found.
 * 
 * @tparam State Type representing problem states
 * @tparam Action Type representing actions/moves
 * @tparam CostType Type for action costs
 * @param problem The problem instance to solve
 * @return Shared pointer to goal node, or nullptr if no solution exists
 */
template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>> IterativeDeepeningSearch(
    Problem<State, Action, CostType> const& problem);

/**
 * @brief Best-First Search algorithm with custom node comparator
 * 
 * Generic framework for search algorithms. Uses a priority queue
 * with a custom comparator to determine node expansion order.
 * 
 * @tparam State Type representing problem states
 * @tparam Action Type representing actions/moves
 * @tparam CostType Type for action costs
 * @tparam NodeComparator Comparator for priority queue ordering
 * @param problem The problem instance to solve
 * @return Shared pointer to goal node, or nullptr if no solution exists
 * 
 * @note Use node_comparators::CompareByPathCost for UCS
 * @note Use node_comparators::CompareByAStar for A* search
 */
template <typename State, typename Action, typename CostType,
          typename NodeComparator>
std::shared_ptr<Node<State, Action, CostType>> BestFirstSearch(
    Problem<State, Action, CostType> const& problem);

/**
 * @brief Uniform Cost Search algorithm
 * 
 * Specialization of Best-First Search that expands nodes in order of
 * path cost g(n). Equivalent to Dijkstra's algorithm for pathfinding.
 * 
 * 
 * @tparam State Type representing problem states
 * @tparam Action Type representing actions/moves
 * @tparam CostType Type for action costs
 * @param problem The problem instance to solve
 * @return Shared pointer to goal node, or nullptr if no solution exists
 * 
 * @note This is a convenience wrapper around BestFirstSearch with path cost comparator
 */
template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>> UniformCostSearch(
    Problem<State, Action, CostType> const& problem) {
    return BestFirstSearch<State, Action, CostType>(problem);
}

}  // namespace search_algorithm

// Include template implementation
#include "best_first_search.tpp"
#include "breadth_first_search.tpp"
#include "depth_first_search.tpp"
#include "depth_limited_search.tpp"
#include "iterative_deepening_search.tpp"
#endif  // SEARCH_ALG_ALGORITHMS_SEARCH_ALGORITHM_H_