#include <limits>
#include <map>
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
    const TPlayer& player, std::map<int, std::vector<TState>>& levels,
    int depth);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, std::map<int, std::vector<TState>>& levels,
    int depth);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MinValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, std::map<int, std::vector<TState>>& levels,
    int depth) {
    levels[depth].push_back(state);

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
            MaxValue<TState, TAction, TUtility, TPlayer>(
                game, *new_state, enemy, levels, depth + 1);

        if (curr_expected_utility < min_expected_utility) {
            min_expected_utility = curr_expected_utility;
            worst_action = std::make_unique<TAction>(std::move(action));
        }
    }

    return {min_expected_utility, std::move(worst_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, std::map<int, std::vector<TState>>& levels,
    int depth) {
    levels[depth].push_back(state);

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
            MinValue<TState, TAction, TUtility, TPlayer>(
                game, *new_state, enemy, levels, depth + 1);

        if (curr_expected_utility > max_expected_utility) {
            max_expected_utility = curr_expected_utility;
            best_action = std::make_unique<TAction>(std::move(action));
        }
    }

    return {max_expected_utility, std::move(best_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::tuple<std::unique_ptr<TAction>, std::map<int, std::vector<TState>>,
           TUtility>
adversarial_search_algorithm::MinimaxSearchStoreTree(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state) {
    std::map<int, std::vector<TState>> levels;

    TPlayer player = game.GetPlayerToMove(state);

    std::unique_ptr<TAction> best_action;
    TUtility value;

    if (player.IsMax())
        std::tie(value, best_action) = MaxValue(game, state, player, levels, 0);
    else
        std::tie(value, best_action) = MinValue(game, state, player, levels, 0);

    return {std::move(best_action), std::move(levels), value};
}