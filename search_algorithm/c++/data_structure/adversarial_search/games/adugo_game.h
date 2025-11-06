#ifndef SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_
#define SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_

#include <array>
#include <memory>
#include <vector>

#include "data_structure/adversarial_search/game.h"

namespace adugo_game {

enum class Symbol : char {
    kEmpty = '*',  // Empty cell
    kBlock = '@',  // Invalid cells
    kC = 'c',      // Cachorro
    kO = 'o',      // Onca
};

class Player {
   public:
    Symbol symbol;
    bool is_max;

    Player() : symbol(Symbol::kEmpty), is_max(false) {}
    Player(Symbol symbol) : symbol(symbol) {
        if (symbol == Symbol::kC)
            is_max = true;
        else
            is_max = false;
    }

    bool IsMax() const { return is_max; }

    // Compare players by their symbol (C or O)
    bool operator==(const Player& other) const {
        return this->symbol == other.symbol;
    }

    bool operator!=(const Player& other) const { return !(*this == other); }
};

const uint8_t kBoardWidth = 5;
const uint8_t kBoardHeight = 7;
const uint8_t kGridDimension = 35;

/**
 * @brief State representing the board configuration as a 1D array.
 * Indexed from * 0 to kGridDimension-1
 * @note Using array instead of vector to define fixed dimension.
 */
using State = std::array<Symbol, kGridDimension>; // 0 to kGridDimension - 1

// Action/Move is a struct that tells where the player is "drawing" its symbol
/**
 * @brief Action representing a player's move on the board.
 * It specifies which player (by its symbol) is making the move and the cell
 * index.
 */
struct Action {
   public:
    // Optimize passa PLayer and not Symbol -> maybe this would be worse
    // actually??
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
 * State: 1D array of 31 Symbols (C, O, Block, Empty)
 * Action: Player's symbol and cell index (0-30)
 * Utility: -1 (loss), 0 (draw), +1 (win) from MAX player's perspective
 * Player: Struct with symbol and IsMax() method
 */
class AdugoGame : public Game<State, Action, Utility, Player> {
   public:
    AdugoGame() : Game(CreateInitialBoard()) {}

    Symbol playerToMove = Symbol::kC;//A ver, n sei mt bem como controlar qm ta jogando

    Player GetPlayerToMove(const State& state) const override;

    std::vector<Action> GetActions(const State& state) const override;
    std::vector<Action> GetPlayerActions(const State& state, Player player) const;
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
    //return the intial board
    static State CreateInitialBoard() {
        State board;
        board.fill(Symbol::kEmpty);
        
        //add blocked cells
        board[kBoardWidth*5+0] = Symbol::kBlock;
        board[kBoardWidth*5+4] = Symbol::kBlock;
        board[kBoardWidth*6+1] = Symbol::kBlock;
        board[kBoardWidth*6+3] = Symbol::kBlock;
        
        //add animals
        for (int i{0}; i<3; ++i)
            for (int j{0}; j<5; ++j)
                board[kBoardWidth*i+j] = Symbol::kC;
        board[kBoardWidth*2+2] = Symbol::kO;

        return board;
    }

    /**
     * @return Player that won. If it's a draw or a non terminal state, the
     * player's symbol will be kEmpty.
     */
    Player CalculateWinner(const State& state) const;
};
};  // namespace adugo_game
#endif  // SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_
