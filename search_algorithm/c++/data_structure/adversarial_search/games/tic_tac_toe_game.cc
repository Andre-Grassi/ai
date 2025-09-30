#include "tic_tac_toe_game.h"

#include <memory>
#include <stdexcept>
#include <vector>

using namespace tic_tac_toe_game;

int TicTacToeGame::GetPlayerToMove(const State& state) const {
    // If the game has ended, no player can play
    if (IsTerminal(state)) return -1;

    uint8_t cross_num = 0;

    // Count number of crosses
    for (Symbol symbol : state)
        if (symbol == Symbol::kX) cross_num++;

    if (cross_num % 2 == 0) return int(Player::kX);

    return int(Player::kO);
}

std::vector<Action> TicTacToeGame::GetActions(const State& state) const {
    std::vector<Action> actions;
    actions.reserve(
        kGridDimension);  // Pre-allocate the maximum number of actions possible

    if (IsTerminal(state)) return actions;  // Return empty vector //DEBUG
    // is it truly empty?

    // Get the player which turn is right now
    Player player = (Player)GetPlayerToMove(state);
    ;

    Symbol player_symbol;
    if (player == Player::kO)
        player_symbol = Symbol::kO;
    else
        player_symbol = Symbol::kX;

    // Attribute for each empty cell a possible action
    for (int i = 0; i < kGridDimension; i++) {
        Symbol cell_symbol = state[i];

        if (cell_symbol == Symbol::kEmpty) {
            Action action(player_symbol, i);
            actions.emplace_back(action);
        }
    }

    return actions;
}

std::unique_ptr<State> TicTacToeGame::GetResult(const State& state,
                                                const Action& action) const {
    if (IsTerminal(state)) return nullptr;

    Player player = (Player)GetPlayerToMove(state);
    Symbol action_symbol = action.player_symbol;
    int ply_index = action.cell_index;

    // Check if the player turn is the same as the symbol being played.
    // Which means that the player X can't play O and vice-versa.
    if ((int)action_symbol != (int)player) return nullptr;

    // Check if the place that the player will play is already occupied
    if (state[ply_index] != Symbol::kEmpty) return nullptr;

    // Valid action
    std::unique_ptr<State> new_state = std::make_unique<State>(
        state);  // Copy the state array to a new pointer

    // Apply the action
    (*new_state)[ply_index] = action_symbol;

    return new_state;
}

bool TicTacToeGame::IsTerminal(const State& state) const {
    bool is_board_full = true;
    // Check if all the cells are non-empty
    for (Symbol cell_symbol : state)
        // If cell is empty, the board has not been completed
        if (cell_symbol == Symbol::kEmpty) is_board_full = false;

    // Check if someone won, even though the board is not completely filled
    if (!is_board_full)
        if (CalculateWinner(state) != Player::kNoPlayer) return false;

    // Game ended on a draw or someone won
    return true;
}

Utility TicTacToeGame::GetUtility(const State& state,
                                  const Player& player) const {
    if (!IsTerminal(state))
        throw std::logic_error("GetUtility called on non-terminal state");

    Player winner = CalculateWinner(state);
    // DEBUG does this comparison work?? Comparing classes
    if (winner == Player::kNoPlayer) return 0;  // Draw
    if (winner == player) return 1;             // Win
    return -1;                                  // Loss
}

Player TicTacToeGame::CalculateWinner(const State& state) const {
    if (!IsTerminal(state))
        return Player::kNoPlayer;  // Non terminal state, no winners

    // Check if there is any line completed
    for (int i = 0; i < kSideSize; i++) {
        Symbol reference_symbol = state[i * kSideSize];
        if (reference_symbol == Symbol::kEmpty) continue;

        Symbol other_symbol = state[i * kSideSize + 1];
        for (int j = 1; j < kSideSize && other_symbol == reference_symbol;
             j++, other_symbol = state[i * kSideSize + j]) {
        }

        if (other_symbol == reference_symbol)
            return static_cast<Player>(reference_symbol);
    }

    // Check if there is any column completed
    for (int i = 0; i < kSideSize; i++) {
        Symbol reference_symbol = state[i];
        if (reference_symbol == Symbol::kEmpty) continue;

        Symbol other_symbol = state[1 * kSideSize + i];
        for (int j = 1; j < kSideSize && other_symbol == reference_symbol;
             j++, other_symbol = state[j * kSideSize + i]) {
        }

        if (other_symbol == reference_symbol)
            return static_cast<Player>(reference_symbol);
    }

    // Check if the main diagonal is completed
    Symbol reference_symbol = state[0];
    Symbol other_symbol = state[kSideSize + 1];
    for (int i = kSideSize + 1;
         i < kGridDimension && reference_symbol == other_symbol;
         i += kSideSize + 1, other_symbol = state[i]) {
    }
    if (other_symbol == reference_symbol)
        return static_cast<Player>(reference_symbol);

    // Check if the secondary diagonal is completed
    Symbol reference_symbol = state[0];
    Symbol other_symbol = state[kSideSize - 1];
    for (int i = kSideSize - 1;
         i < kGridDimension && reference_symbol == other_symbol;
         i += kSideSize - 1, other_symbol = state[i]) {
    }
    if (other_symbol == reference_symbol)
        return static_cast<Player>(reference_symbol);

    // Draw
    return Player::kNoPlayer;
}