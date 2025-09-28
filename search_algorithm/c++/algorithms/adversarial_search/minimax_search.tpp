#include <limits>
#include <tuple>
#include <utility>  // for std::pair, not the Utility from the game

#include "adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/game.h"

using namespace adversarial_search_algorithm;

// Auxiliary functions
template <typename TState, typename TAction, typename TUtility>
std::pair<TUtility, std::unique_ptr<TAction>> MinValue(
    const Game<TState, TAction, TUtility>& game, const TState& state,
    const int player_index) {
    // In terminal state, just return the utility for the player, because there
    // are no more moves to make
    if (game.IsTerminal(state)) {
        TUtility utility = game.GetUtility(state, player_index);
        nullptr_t no_action = nullptr;
        return std::make_pair<TUtility, std::unique_ptr<TAction>>(utility,
                                                                  no_action);
    }

    TUtility min_expected_utility =
        std::numeric_limits<TUtility>::inifity();  // + inf
    std::unique_ptr<TAction> worst_action = std::make_unique<TAction>();

    for (TAction action : game.GetActions(state)) {
        TUtility curr_utility;
        std::unique_ptr<TAction> curr_action;

        std::tie<TUtility, std::unique_ptr<TAction>>(curr_utility,
                                                     curr_action) =
            MaxValue(game, game.GetResult(state, curr_action), player_index);

        if (curr_utility < min_expected_utility) {
            min_expected_utility = curr_utility;
            worst_action = curr_action;
        }
    }

    return std::make_pair<TUtility, std::unique_ptr<TAction>>(
        min_expected_utility, worst_action);
}

template <typename TState, typename TAction, typename TUtility>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility>& game, const TState& state,
    const int player_index) {
    // In terminal state, just return the utility for the player, because there
    // are no more moves to make
    if (game.IsTerminal(state)) {
        TUtility utility = game.GetUtility(state, player_index);
        nullptr_t no_action = nullptr;
        return std::make_pair<TUtility, std::unique_ptr<TAction>>(utility,
                                                                  no_action);
    }

    TUtility max_expected_utility =
        -std::numeric_limits<TUtility>::infinity();  // - inf
    std::unique_ptr<TAction> best_action = std::make_unique<TAction>();

    // Get the maximum of the minimum utilities (considering that the adversary
    // plays optimally) and the corresponding action
    for (TAction action : game.GetActions(state)) {
        TUtility curr_utility;
        std::unique_ptr<TAction> curr_action;
        std::tie<TUtility, std::unique_ptr<TAction>>(curr_utility,
                                                     curr_action) =
            MinValue<TState, std::unique_ptr<TAction>, TUtility>(
                game, game.GetResult(state, curr_action), player_index);

        if (curr_utility > max_expected_utility) {
            max_expected_utility = curr_utility;
            best_action = curr_action;
        }
    }

    return std::make_pair<TUtility, std::unique_ptr<TAction>>(
        max_expected_utility, best_action);
}

template <typename TState, typename TAction, typename TUtility>
std::unique_ptr<TAction> adversarial_search_algorithm::MinimaxSearch(
    const Game<TState, TAction, TUtility>& game, const TState& state) {
    int player_index = game.GetPlayerToMove(state);

    std::unique_ptr<TAction> best_action;

    std::tie<TUtility, std::unique_ptr<TAction>>(std::ignore, best_action) =
        MaxValue(game, state, player_index);

    return best_action;
}