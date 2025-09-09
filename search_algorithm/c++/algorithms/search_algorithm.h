#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "../data_structure/problems/sliding_tile_problem.h"

namespace search_algorithm {
template <typename State, typename Action>
Node<State, Action>* BreadthFirstSearch(Problem<State, Action> const& problem);
}