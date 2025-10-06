#include "tic_tac_toe_game.h"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace tic_tac_toe_game;

Player TicTacToeGame::GetPlayerToMove(const State& state) const {
    // If the game has ended, no player can play
    if (IsTerminal(state)) return Player(Symbol::kEmpty);

    uint8_t cross_num = 0;
    uint8_t nought_num = 0;

    // Count number of crosses and noughts
    for (Symbol symbol : state)
        if (symbol == Symbol::kX)
            cross_num++;
        else if (symbol == Symbol::kO)
            nought_num++;

    if (cross_num == 0) return Player(Symbol::kX);  // Empty board

    if (cross_num <= nought_num)
        return Player(Symbol::kX);
    else
        return Player(Symbol::kO);
}

std::vector<Action> TicTacToeGame::GetActions(const State& state) const {
    std::vector<Action> actions;
    actions.reserve(
        kGridDimension);  // Pre-allocate the maximum number of actions possible

    if (IsTerminal(state)) {
        return actions;
    }  // Return empty vector //DEBUG
    // is it truly empty?

    // Get the player which turn is right now
    Player player = static_cast<Player>(GetPlayerToMove(state));
    ;

    // Attribute for each empty cell a possible action
    for (int i = 0; i < kGridDimension; i++) {
        Symbol cell_symbol = state[i];

        if (cell_symbol == Symbol::kEmpty) {
            Action action(player.symbol, i);
            actions.emplace_back(action);
        }
    }

    return actions;
}

std::unique_ptr<State> TicTacToeGame::GetResult(const State& state,
                                                const Action& action) const {
    if (IsTerminal(state)) {
        return nullptr;
    }

    Player player = GetPlayerToMove(state);
    Symbol action_symbol = action.player_symbol;
    int ply_index = action.cell_index;

    // Check if the player turn is the same as the symbol being played.
    // Which means that the player X can't play O and vice-versa.
    if (action_symbol != player.symbol) {
        return nullptr;
    }

    // Check if the place that the player will play is already occupied
    if (state[ply_index] != Symbol::kEmpty) {
        return nullptr;
    }

    // Valid action
    std::unique_ptr<State> new_state = std::make_unique<State>(
        state);  // Copy the state array to a new pointer

    // Apply the action
    (*new_state)[ply_index] = action_symbol;

    return new_state;
}

bool TicTacToeGame::IsTerminal(const State& state) const {
    bool is_board_full = true;
    bool is_board_empty = true;

    // Check if the board is full or empty
    for (Symbol cell_symbol : state)
        if (cell_symbol == Symbol::kEmpty)
            is_board_full = false;  // Non-full, there are empty cells
        else
            is_board_empty = false;  // Non-empty, there are cells occupied

    // Check if someone won, even though the board is not completely filled
    if (!is_board_full) {
        if (is_board_empty) return false;  // Empty board -> non terminal
        if ((CalculateWinner(state)).symbol != Symbol::kEmpty)
            return true;  // Board not full, and we have a winner -> terminal
        return false;     // Board not full, and no winner -> non terminal
    }

    // Game ended on a draw or someone won -> terminal
    return true;
}

Utility TicTacToeGame::GetUtility(const State& state) const {
    if (!IsTerminal(state))
        throw std::logic_error("GetUtility called on non-terminal state");

    Player winner = CalculateWinner(state);

    if (winner == Player(Symbol::kEmpty)) return 0;  // Draw
    if (winner == Player(Symbol::kX)) return 1;      // X Win
    return -1;                                       // O win
}

Player TicTacToeGame::CalculateWinner(const State& state) const {
    Symbol reference_symbol;

    // Check if there is any line completed
    for (int i = 0; i < kSideSize; i++) {
        reference_symbol = state[i * kSideSize];
        if (reference_symbol == Symbol::kEmpty) continue;

        int j;
        for (j = 1;
             j < kSideSize && reference_symbol == state[i * kSideSize + j];
             j++) {
        }

        if (j >= kSideSize) return static_cast<Player>(reference_symbol);
    }

    // Check if there is any column completed
    for (int i = 0; i < kSideSize; i++) {
        reference_symbol = state[i];
        if (reference_symbol == Symbol::kEmpty) continue;

        int j;
        for (j = 1;
             j < kSideSize && reference_symbol == state[j * kSideSize + i];
             j++) {
        }

        if (j >= kSideSize) return static_cast<Player>(reference_symbol);
    }

    // Check if the main diagonal is completed
    reference_symbol = state[0];
    if (reference_symbol != Symbol::kEmpty) {
        int i;
        for (i = kSideSize + 1;
             i < kGridDimension && reference_symbol == state[i];
             i += kSideSize + 1) {
        }

        if (i >= kGridDimension) return static_cast<Player>(reference_symbol);
    }

    // Check if the secondary diagonal is completed
    reference_symbol = state[kSideSize - 1];
    if (reference_symbol != Symbol::kEmpty) {
        int i;
        for (i = kSideSize - 1 + kSideSize - 1;
             i <= kGridDimension - 1 - (kSideSize - 1) &&
             reference_symbol == state[i];
             i += kSideSize - 1) {
        }

        if (i > kGridDimension - 1 - (kSideSize - 1))
            return static_cast<Player>(reference_symbol);
    }

    // Draw (but if it's not a terminal state, then this is not a draw)
    return Player(Symbol::kEmpty);
}

std::string TicTacToeGame::GetStateString(const State& state) const {
    std::stringstream state_ss;
    for (int i = 0; i < kGridDimension; i += kSideSize) {
        for (int j = 0; j < kSideSize; j++) {
            char player_char = '_';
            if (state[i + j] == Symbol::kO)
                player_char = 'O';
            else if (state[i + j] == Symbol::kX)
                player_char = 'X';

            state_ss << player_char;
        }
        state_ss << std::endl;
    }

    return state_ss.str();
}