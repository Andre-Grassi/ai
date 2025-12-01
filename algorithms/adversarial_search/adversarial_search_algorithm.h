#ifndef ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_
#define ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_

#include <limits>
#include <map>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "data_structure/adversarial_search/game.h"

namespace adversarial_search_algorithm {
// Helper functions
template <typename T>
constexpr T positive_infinity() {
    if constexpr (std::numeric_limits<T>::has_infinity)
        return std::numeric_limits<T>::infinity();
    else
        return std::numeric_limits<T>::max();
}

template <typename T>
constexpr T negative_infinity() {
    if constexpr (std::numeric_limits<T>::has_infinity)
        return -std::numeric_limits<T>::infinity();
    else
        return std::numeric_limits<T>::lowest();
}

// Heuristic Alpha-Beta pruning Minimax with transposition table (can avoid
// cycles)
template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::unique_ptr<TAction> HeuristicMinimaxSearch(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    std::unordered_map<TState, TUtility>& transposition_table);
}  // namespace adversarial_search_algorithm

#include "heuristic_minimax.tpp"

#endif  // ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_