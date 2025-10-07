#ifndef SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_TIC_TAC_TOE_GAME_H_
#define SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_TIC_TAC_TOE_GAME_H_

#include <array>
#include <memory>
#include <vector>

#include "data_structure/adversarial_search/game.h"

namespace tic_tac_toe_game {

/**
 * @brief Symbol that each player uses to mark the board.
 * The kEmpty symbol is a "wild card", can represent empty cells of the board
 * and also a draw (more explanation about this in the class).
 */
enum class Symbol {
    kX = 1,      // Cross (X)
    kO = -1,     // Nought (O)
    kEmpty = 0,  // Empty cell
};

/**
 * @note The Max player will always be the player using X symbol
 * and the Min player will always be the player using O symbol.
 */
class Player {
   public:
    Symbol symbol;
    bool is_max;

    Player() : symbol(Symbol::kEmpty), is_max(false) {}
    Player(Symbol symbol) : symbol(symbol) {
        if (symbol == Symbol::kX)
            is_max = true;
        else
            is_max = false;
    }

    bool IsMax() const { return is_max; }

    // Compare players by their symbol (X or O)
    bool operator==(const Player& other) const {
        return this->symbol == other.symbol;
    }

    bool operator!=(const Player& other) const { return !(*this == other); }
};

const uint8_t kSideSize = 3;  ///< Number of cells per side of the board
const uint8_t kGridDimension =
    kSideSize * kSideSize;  ///< Total number of cells

/**
 * @brief State representing the board configuration as a 1D array.
 * Indexed from * 0 to kGridDimension-1
 * @note Using array instead of vector to define fixed dimension.
 */
using State =
    std::array<Symbol, kGridDimension>;  // 0 to kGridDimension - 1 indexing

// Action/Move is a struct that tells where the player is "drawing" its symbol
/**
 * @brief Action representing a player's move on the board.
 * It specifies which player (by its symbol) is making the move and the cell
 * index.
 */
struct Action {
   public:
    // FIX passa PLayer and not Symbol
    Symbol player_symbol;
    int cell_index;

    Action() : player_symbol(Symbol::kEmpty), cell_index(-1) {}

    Action(Symbol player_symbol, int cell_index)
        : player_symbol(player_symbol), cell_index(cell_index) {};
};

/**
 * @note For debugging, use int, because the debugger shows int8_t as char.
 */
using Utility = int8_t;  // -1 (loss), 0 (draw), +1 (win)

/**
 * @brief Class representing the Tic-Tac-Toe game.
 *
 * Implements the Game interface for a standard 3x3 Tic-Tac-Toe game.
 * Players alternate placing X and O on the board, aiming to get three in a
 * row. The game ends when one player wins or the board is full (draw).
 *
 * State: 1D array of 9 Symbols (X, O, Empty)
 * Action: Player's symbol and cell index (0-8)
 * Utility: -1 (loss), 0 (draw), +1 (win) from MAX player's perspective
 * Player: Struct with symbol and IsMax() method
 */
class TicTacToeGame : public Game<State, Action, Utility, Player> {
   public:
    TicTacToeGame() : Game(CreateEmptyBoard()) {}

    Player GetPlayerToMove(const State& state) const override;

    std::vector<Action> GetActions(const State& state) const override;

    std::unique_ptr<State> GetResult(const State& state,
                                     const Action& action) const override;

    /**
     * @return True if the game is over (win or draw), false otherwise
     */
    bool IsTerminal(const State& state) const override;

    /**
     * @return +1 if MAX (X) wins, -1 if MIN (O) wins, 0 for draw
     * @warning Only call on terminal states
     */
    Utility GetUtility(const State& state) const override;

    virtual std::string GetStateString(const State& state) const override;

   private:
    static State CreateEmptyBoard() {
        State board;
        board.fill(Symbol::kEmpty);
        return board;
    }

    /**
     * @return Player that won. If it's a draw or a non terminal state, the
     * player's symbol will be kEmpty.
     */
    Player CalculateWinner(const State& state) const;
};
};  // namespace tic_tac_toe_game
#endif  // SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_TIC_TAC_TOE_GAME_H_