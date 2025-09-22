/**
 * @file search_algorithm.h
 * @brief Collection of AI search visual algorithm implementations
 * @author Andre Grassi
 * @date 2025
 */

#ifndef SEARCH_ALG_ALGORITHMS_VISUAL_VISUAL_SEARCH_H_
#define SEARCH_ALG_ALGORITHMS_VISUAL_VISUAL_SEARCH_H_

#include "../../data_structure/node.h"
#include "../../data_structure/node_comparators.h"
#include "../../data_structure/problem.h"
#include "../../data_structure/problems/sliding_tile_problem.h"
#include "../../data_structure/visual/visual_node.h"

namespace visual_search {

template <typename State, typename Action, typename CostType>
std::shared_ptr<VisualNode<State, Action, CostType>> VisualBreadthFirstSearch(
    Problem<State, Action, CostType> const& problem);
}  // namespace visual_search

// Include template implementation
#include "visual_breadth_first.tpp"
#endif  // SEARCH_ALG_ALGORITHMS_VISUAL_VISUAL_SEARCH_H_