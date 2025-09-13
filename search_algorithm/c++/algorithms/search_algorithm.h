#ifndef SEARCH_ALGORITHM_BREADTH_FIRST_SEARCH_H
#define SEARCH_ALGORITHM_BREADTH_FIRST_SEARCH_H

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
template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> DepthLimitedSearch(
    Problem<State, Action> const& problem, uint64_t depth_limit,
    bool check_node_cycles);
}  // namespace search_algorithm

// Include template implementation
#include "breadth_first_search.tpp"
#include "depth_first_search.tpp"
#include "depth_limited_search.tpp"
#endif  // SEARCH_ALGORITHM_BREADTH_FIRST_SEARCH_H