#ifndef ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_
#define ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_

#include "data_structure/adversarial_search/game.h"

namespace adversarial_search_algorithm {
template <typename TState, typename TAction, typename TUtility>
std::unique_ptr<TAction> MinimaxSearch(
    const Game<TState, TAction, TUtility>& game, const TState& state);

}  // namespace adversarial_search_algorithm

#include "minimax_search.tpp"

#endif  // ALGORITHMS_ADVERSARIAL_SEARCH_ADVERSARIAL_SEARCH_ALGORITHM_H_