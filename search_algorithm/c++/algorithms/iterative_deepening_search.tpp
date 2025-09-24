#include <set>
#include <stack>
#include <vector>

#include "data_structure/node.h"
#include "data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.12, page 99, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action, typename CostType>
std::shared_ptr<Node<State, Action, CostType>> search_algorithm::IterativeDeepeningSearch(
    Problem<State, Action, CostType> const& problem) {
    std::shared_ptr<Node<State, Action, CostType>> result = nullptr;

    // Increase depth limit until solution is found
    for (uint64_t depth = 0;; ++depth) {
        bool cutoff_occurred = false;
        result = DepthLimitedSearch(problem, depth, true, &cutoff_occurred);
        if (!cutoff_occurred)
            return result;  // Solution found or it doesn't exist
    }
}