#ifndef SEARCH_ALGORITHM_BREADTH_FIRST_SEARCH_H
#define SEARCH_ALGORITHM_BREADTH_FIRST_SEARCH_H

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "../data_structure/problems/sliding_tile_problem.h"

namespace search_algorithm {
template <typename State, typename Action>
Node<State, Action>* BreadthFirstSearch(Problem<State, Action> const& problem);
}

// Include template implementation
#include "breadth_first_search.tpp"
#endif  // SEARCH_ALGORITHM_BREADTH_FIRST_SEARCH_H