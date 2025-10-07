#ifndef ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_
#define ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_

#include <limits>
#include <map>
#include <tuple>
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

// Minimax
template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::unique_ptr<TAction> MinimaxSearch(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::tuple<std::unique_ptr<TAction>, std::map<int, std::vector<TState>>,
           TUtility>
MinimaxSearchStoreTree(const Game<TState, TAction, TUtility, TPlayer>& game,
                       const TState& state);

}  // namespace adversarial_search_algorithm

#include "minimax_search.tpp"
#include "minimax_search_store_tree.tpp"

#endif  // ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_