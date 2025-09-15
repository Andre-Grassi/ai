#ifndef SEARCH_ALG_ALGORITHMS_SEARCH_ALGORITHM_H_
#define SEARCH_ALG_ALGORITHMS_SEARCH_ALGORITHM_H_

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "../data_structure/problems/sliding_tile_problem.h"

namespace search_algorithm {
template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> BreadthFirstSearch(
    Problem<State, Action> const& problem);

template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> DepthFirstSearch(
    Problem<State, Action> const& problem);

// check_node_cycles: if true, check for cycles in the current path to avoid
// redundant paths, but is more computationally expensive
// *out_cutoff: if not nullptr, set to true if a cutoff occurred and no solution
// was found
template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> DepthLimitedSearch(
    Problem<State, Action> const& problem, uint64_t depth_limit,
    bool check_node_cycles, bool* out_cutoff);

// Calls DepthLimitedSearch with increasing depth limits until a solution is
// found or it is determined that no solution exists
template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> IterativeDeepeningSearch(
    Problem<State, Action> const& problem);

template <typename State, typename Action, typename NodeComparator>
std::shared_ptr<Node<State, Action>> BestFirstSearch(
    Problem<State, Action> const& problem);
}  // namespace search_algorithm

// Include template implementation
#include "best_first_search.tpp"
#include "breadth_first_search.tpp"
#include "depth_first_search.tpp"
#include "depth_limited_search.tpp"
#include "iterative_deepening_search.tpp"
#endif  // SEARCH_ALG_ALGORITHMS_SEARCH_ALGORITHM_H_