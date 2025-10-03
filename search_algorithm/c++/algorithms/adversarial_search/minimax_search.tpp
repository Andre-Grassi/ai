#include <limits>
#include <tuple>
#include <utility>  // for std::pair, not the Utility from the game

#include "adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/game.h"

using namespace adversarial_search_algorithm;

// Auxiliary functions
template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MinValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MinValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player) {
    // In terminal state, just return the utility for the player, because there
    // are no more moves to make
    if (game.IsTerminal(state)) {
        TUtility utility = game.GetUtility(state, player);
        // Using nullptr to indicate that there is no action to be taken
        return {utility, nullptr};
    }

    TUtility min_expected_utility =
        std::numeric_limits<TUtility>::infinity();  // + inf
    std::unique_ptr<TAction> worst_action = std::make_unique<TAction>();

    for (TAction action : game.GetActions(state)) {
        TUtility curr_expected_utility;

        std::tie(curr_expected_utility, std::ignore) =
            MaxValue<TState, TAction, TUtility, TPlayer>(
                game, *(game.GetResult(state, action)), player);

        if (curr_expected_utility < min_expected_utility) {
            min_expected_utility = curr_expected_utility;
            *worst_action = action;
        }
    }

    return {min_expected_utility, std::move(worst_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player) {
    // In terminal state, just return the utility for the player, because there
    // are no more moves to make
    if (game.IsTerminal(state)) {
        TUtility utility = game.GetUtility(state, player);
        // Using nullptr to indicate that there is no action to be taken
        return {utility, nullptr};
    }

    TUtility max_expected_utility =
        -std::numeric_limits<TUtility>::infinity();  // - inf
    std::unique_ptr<TAction> best_action = std::make_unique<TAction>();

    // Get the maximum of the minimum utilities (considering that the adversary
    // plays optimally) and the corresponding action
    for (TAction action : game.GetActions(state)) {
        TUtility curr_expected_utility;
        std::tie(curr_expected_utility, std::ignore) =
            MinValue<TState, TAction, TUtility, TPlayer>(
                game, *(game.GetResult(state, action)), player);

        if (curr_expected_utility > max_expected_utility) {
            max_expected_utility = curr_expected_utility;
            *best_action = action;
        }
    }

    return {max_expected_utility, std::move(best_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::unique_ptr<TAction> adversarial_search_algorithm::MinimaxSearch(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state) {
    TPlayer player = game.GetPlayerToMove(state);

    std::unique_ptr<TAction> best_action;

    // std::tie without template parameters to let it deduce.
    // An error was ocurring because of the std::ignore, it probably has a
    // specific type
    std::tie(std::ignore, best_action) = MaxValue(game, state, player);

    return best_action;
}