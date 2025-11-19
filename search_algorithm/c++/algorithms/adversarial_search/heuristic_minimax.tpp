#include <algorithm>
#include <limits>
#include <tuple>
#include <unordered_map>
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
    const TPlayer& player, TUtility alpha, TUtility beta,
    std::unordered_map<TState, TUtility>& transposition_table);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, TUtility alpha, TUtility beta,
    std::unordered_map<TState, TUtility>& transposition_table);

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MinValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, TUtility alpha, TUtility beta, int depth,
    std::unordered_map<TState, TUtility>& transposition_table) {
    // When the depth cutoff is reached or it's a terminal state, just return
    // the expected value (or utility, if terminal).
    if (game.IsCutoff(state, depth)) {
        TUtility value = game.GetEval(state);
        // Using nullptr to indicate that there is no action to be taken
        return {value, nullptr};
    }

    TUtility min_expected_value = positive_infinity<TUtility>();
    std::unique_ptr<TAction> worst_action;

    for (TAction action : game.GetActions(state)) {
        TUtility curr_expected_value;
        std::unique_ptr<TState> new_state = game.GetResult(state, action);

        auto it = transposition_table.find(*new_state);
        if (it != transposition_table.end())
            // Key exists
            curr_expected_value = it->second;
        else {
            TPlayer enemy = game.GetPlayerToMove(*new_state);

            std::tie(curr_expected_value, std::ignore) =
                MaxValue<TState, TAction, TUtility, TPlayer>(
                    game, *new_state, enemy, alpha, beta, depth + 1,
                    transposition_table);
            transposition_table[*new_state] = curr_expected_value;
        }

        if (curr_expected_value < min_expected_value) {
            min_expected_value = curr_expected_value;
            worst_action = std::make_unique<TAction>(std::move(action));
            beta = std::min<TUtility>(beta, min_expected_value);
        }
        if (min_expected_value <= alpha)
            return {min_expected_value, std::move(worst_action)};
    }

    return {min_expected_value, std::move(worst_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::pair<TUtility, std::unique_ptr<TAction>> MaxValue(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    const TPlayer& player, TUtility alpha, TUtility beta, int depth,
    std::unordered_map<TState, TUtility>&
        transposition_table) {  // Google suggets int instead of size_t or
                                // unsigned
    // When the depth cutoff is reached or it's a terminal state, just return
    // the expected value (or utility, if terminal).
    if (game.IsCutoff(state, depth)) {
        TUtility value = game.GetEval(state);
        // Using nullptr to indicate that there is no action to be taken
        return {value, nullptr};
    }

    TUtility max_expected_value = negative_infinity<TUtility>();
    std::unique_ptr<TAction> best_action;

    std::vector<TAction> actions = game.GetActions(state);
    for (TAction action : actions) {
        TUtility curr_expected_value;
        std::unique_ptr<TState> new_state = game.GetResult(state, action);

        auto it = transposition_table.find(*new_state);
        if (it != transposition_table.end())
            // Key exists
            curr_expected_value = it->second;
        else {
            TPlayer enemy = game.GetPlayerToMove(*new_state);

            std::tie(curr_expected_value, std::ignore) =
                MinValue<TState, TAction, TUtility, TPlayer>(
                    game, *new_state, enemy, alpha, beta, depth + 1,
                    transposition_table);

            transposition_table[*new_state] = curr_expected_value;
        }

        if (curr_expected_value > max_expected_value) {
            max_expected_value = curr_expected_value;
            best_action = std::make_unique<TAction>(std::move(action));
            alpha = std::max<TUtility>(alpha, max_expected_value);
        }
        if (max_expected_value >= beta)
            return {max_expected_value, std::move(best_action)};
    }

    return {max_expected_value, std::move(best_action)};
}

template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
std::unique_ptr<TAction> adversarial_search_algorithm::HeuristicMinimaxSearch(
    const Game<TState, TAction, TUtility, TPlayer>& game, const TState& state,
    std::unordered_map<TState, TUtility>& transposition_table) {
    TPlayer player = game.GetPlayerToMove(state);
    std::unique_ptr<TAction> best_action;
    int initial_depth = 0;

    // OPTIMIZE pass transposition_table by reference?
    if (player.IsMax())
        // std::tie without template parameters to let it deduce.
        // An error was ocurring because of the std::ignore, it probably has
        // a specific type
        std::tie(std::ignore, best_action) = MaxValue(
            game, state, player, negative_infinity<TUtility>(),
            positive_infinity<TUtility>(), initial_depth, transposition_table);
    else
        std::tie(std::ignore, best_action) = MinValue(
            game, state, player, negative_infinity<TUtility>(),
            positive_infinity<TUtility>(), initial_depth, transposition_table);
    return best_action;
}