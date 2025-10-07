#include <algorithm>
#include <limits>
#include <tuple>
#include <utility>  // for std::pair, not the Utility from the game

#include "adversarial_search_algorithm.h"
#include "data_structure/adversarial_search/game.h"
#include "data_structure/adversarial_search/games/tic_tac_toe_game.h"

using namespace adversarial_search_algorithm;

// Auxiliary functions
template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MinValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, TUtility alpha, TUtility beta);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, TUtility alpha, TUtility beta);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MinValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, TUtility alpha, TUtility beta) {
    // In terminal state, just return the utility for the player, because there
    // are no more moves to make
    if (game.IsTerminal(state)) {
        TUtility utility = game.GetUtility(state);
        // Using nullptr to indicate that there is no action to be taken
        return {utility, nullptr};
    }

    TUtility min_expected_utility = positive_infinity<TUtility>();
    std::unique_ptr<TAction> worst_action;

    for (TAction action : game.GetActions(state)) {
        TUtility curr_expected_utility;
        std::unique_ptr<TState> new_state = game.GetResult(state, action);
        TPlayer enemy = game.GetPlayerToMove(*new_state);

        std::tie(curr_expected_utility, std::ignore) =
            MaxValue<TState, TAction, TUtility, TPlayer>(game, *new_state,
                                                         enemy);

        if (curr_expected_utility < min_expected_utility) {
            min_expected_utility = curr_expected_utility;
            worst_action = std::make_unique<TAction>(std::move(action));
            beta = std::min<TUtility>(beta, min_expected_utility);
        }
        if (min_expected_utility <= alpha)
            return {min_expected_utility, std::move(worst_action)};
    }

    return {min_expected_utility, std::move(worst_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, TUtility alpha, TUtility beta) {
    // In terminal state, just return the utility for the player, because there
    // are no more moves to make
    if (game.IsTerminal(state)) {
        TUtility utility = game.GetUtility(state);
        // Using nullptr to indicate that there is no action to be taken
        return {utility, nullptr};
    }

    TUtility max_expected_utility = negative_infinity<TUtility>();
    std::unique_ptr<TAction> best_action;

    std::vector<TAction> actions = game.GetActions(state);
    for (TAction action : actions) {
        TUtility curr_expected_utility;
        std::unique_ptr<TState> new_state = game.GetResult(state, action);
        TPlayer enemy = game.GetPlayerToMove(*new_state);

        std::tie(curr_expected_utility, std::ignore) =
            MinValue<TState, TAction, TUtility, TPlayer>(game, *new_state,
                                                         enemy, alpha, beta);

        if (curr_expected_utility > max_expected_utility) {
            max_expected_utility = curr_expected_utility;
            best_action = std::make_unique<TAction>(std::move(action));
            alpha = std::max<TUtility>(alpha, max_expected_utility);
        }
        if (max_expected_utility >= beta)
            return {max_expected_utility, std::move(best_action)};
    }

    return {max_expected_utility, std::move(best_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::unique_ptr<TAction> adversarial_search_algorithm::MinimaxSearchWithPruning(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state) {
    TPlayer player = game.GetPlayerToMove(state);

    std::unique_ptr<TAction> best_action;

    if (player.IsMax())
        // std::tie without template parameters to let it deduce.
        // An error was ocurring because of the std::ignore, it probably has a
        // specific type
        std::tie(std::ignore, best_action) =
            MaxValue(game, state, player, negative_infinity<TUtility>(),
                     positive_infinity<TUtility>());
    else
        std::tie(std::ignore, best_action) =
            MinValue(game, state, player, negative_infinity<TUtility>(),
                     positive_infinity<TUtility>());

    return best_action;
}