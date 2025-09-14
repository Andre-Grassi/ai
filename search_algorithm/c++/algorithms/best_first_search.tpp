#include <set>
#include <stack>
#include <vector>

#include "../data_structure/node.h"
#include "../data_structure/problem.h"
#include "search_algorithm.h"

using namespace search_algorithm;

// Reference: figure 3.12, page 99, Artificial Intelligence: A Modern Approach,
// 4th edition

template <typename State, typename Action>
std::shared_ptr<Node<State, Action>> search_algorithm::BestFirstSearch(
    Problem<State, Action> const& problem) {
    State initialState = problem.GetInitialState();
    std::shared_ptr<Node<State, Action>> root =
        std::make_shared<Node<State, Action>>(initialState);

    std::priority_queue<std::shared_ptr<Node<State, Action>>,
                        std::vector<std::shared_ptr<Node<State, Action>>>>
        frontier = std::priority_queue<
            std::shared_ptr<Node<State, Action>>,
            std::vector<std::shared_ptr<Node<State, Action>>>>();

    // Increase depth limit until solution is found
    for (uint64_t depth = 0;; ++depth) {
        bool cutoff_occurred = false;
        result = DepthLimitedSearch(problem, depth, true, &cutoff_occurred);
        if (!cutoff_occurred)
            return result;  // Solution found or it doesn't exist
    }
}