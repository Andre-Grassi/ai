#ifndef SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_TIC_TAC_TOE_GAME_H_
#define SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_TIC_TAC_TOE_GAME_H_

#include <array>
#include <memory>
#include <vector>

#include "data_structure/adversarial_search/game.h"

namespace tic_tac_toe_game {

enum class Symbol {
    kX = 1,      // Cross
    kO = -1,     // Nought
    kEmpty = 0,  // Empty cell
};

// Player and symbols must be coherent, which means that
// (int)Symbol::kX == (int)Player::kX and the same for kO
enum class Player { kX = 1, kO = -1, kNoPlayer = 0 };

const uint8_t kSideSize = 3;
const uint8_t kGridDimension = kSideSize * kSideSize;

// State is the board configuration
using State =
    std::array<Symbol, kGridDimension>;  // 0 to kGridDimension - 1 indexing

// Action/Move is a struct that tells where the player is "drawing" its symbol
struct Action {
   public:
    int cell_index;
    Symbol player_symbol;

    Action(Symbol player_symbol, int cell_index)
        : player_symbol(player_symbol), cell_index(cell_index) {};
};

using Utility = int8_t;  // -1 (loss), 0 (draw), +1 (win)

class TicTacToeGame : public Game<State, Action, Utility> {
   public:
    TicTacToeGame() : Game(CreateEmptyBoard()) {}

    // If even number of X -> X player will play, otherwise its O turn
    int GetPlayerToMove(const State& state) const override;

    // Return vector with the position of all the empty cells
    std::vector<Action> GetActions(const State& state) const override;

    std::unique_ptr<State> GetResult(const State& state,
                                     const Action& action) const override;

    // True if no cell is equal to kEmpty or if someone won
    bool IsTerminal(const State& state) const override;

    // Player 0 = X
    // Player 1 = O
    Utility GetUtility(const State& state, int player_index) const override;

   private:
    static State CreateEmptyBoard() {
        State board;
        board.fill(Symbol::kEmpty);
        return board;
    }

    // @return 1 -> player X won, -1 -> pX lost, 0 -> draw or non terminal state
    Player CheckWin(const State& state, int player_index) const;
};
};  // namespace tic_tac_toe_game
#endif  // SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_TIC_TAC_TOE_GAME_H_