#ifndef SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_
#define SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "data_structure/adversarial_search/game.h"

namespace adugo_game {
// ---------- nomes para line IDs ----------
constexpr int HORIZONTAL_1 = 1;
constexpr int HORIZONTAL_2 = 2;
constexpr int HORIZONTAL_3 = 3;
constexpr int HORIZONTAL_4 = 4;
constexpr int HORIZONTAL_5 = 5;

constexpr int VERTICAL_1 = 6;
constexpr int VERTICAL_2 = 7;
constexpr int VERTICAL_3 = 8;
constexpr int VERTICAL_4 = 9;
constexpr int VERTICAL_5 = 10;

// diagonais / linhas extras (IDs 11..25)
constexpr int DIAGONAL_11 = 11;
constexpr int DIAGONAL_12 = 12;
constexpr int DIAGONAL_13 = 13;
constexpr int DIAGONAL_14 = 14;
constexpr int DIAGONAL_15 = 15;
constexpr int DIAGONAL_16 = 16;
constexpr int DIAGONAL_17 = 17;
constexpr int DIAGONAL_18 = 18;
constexpr int DIAGONAL_19 = 19;
constexpr int DIAGONAL_20 = 20;
constexpr int DIAGONAL_21 = 21;
constexpr int DIAGONAL_22 = 22;
constexpr int DIAGONAL_23 = 23;
constexpr int DIAGONAL_24 = 24;
constexpr int DIAGONAL_25 = 25;

// ---------- tabela ----------
// formato: node_index -> { {neighbors...}, {line_ids...} }
const std::map<int, std::pair<std::vector<int>, std::vector<int>>>
    kGridDimensionNeighborhood = {

        // Linha 1
        {0, {{1, 5, 6}, {HORIZONTAL_1, VERTICAL_1, DIAGONAL_11}}},
        {1,
         {{0, 2, 6, 7}, {HORIZONTAL_1, VERTICAL_1, DIAGONAL_11, DIAGONAL_12}}},
        {2,
         {{1, 3, 6, 7, 8},
          {HORIZONTAL_1, VERTICAL_1, DIAGONAL_12, DIAGONAL_13}}},
        {3,
         {{2, 4, 8, 9}, {HORIZONTAL_1, VERTICAL_1, DIAGONAL_13, DIAGONAL_14}}},
        {4, {{3, 9}, {HORIZONTAL_1, VERTICAL_1, DIAGONAL_14}}},

        // Linha 2
        {5,
         {{0, 6, 10, 11},
          {HORIZONTAL_2, VERTICAL_1, DIAGONAL_11, DIAGONAL_15}}},
        {6,
         {{0, 1, 2, 5, 7, 10, 11, 12},
          {HORIZONTAL_2, VERTICAL_1, VERTICAL_2, DIAGONAL_11, DIAGONAL_12,
           DIAGONAL_15, DIAGONAL_16}}},
        {7,
         {{1, 2, 3, 6, 8, 11, 12, 13},
          {HORIZONTAL_2, VERTICAL_2, VERTICAL_3, DIAGONAL_12, DIAGONAL_13,
           DIAGONAL_16, DIAGONAL_17}}},
        {8,
         {{2, 3, 4, 7, 9, 12, 13, 14},
          {HORIZONTAL_2, VERTICAL_3, VERTICAL_4, DIAGONAL_13, DIAGONAL_14,
           DIAGONAL_17, DIAGONAL_18}}},
        {9,
         {{3, 4, 8, 13, 14},
          {HORIZONTAL_2, VERTICAL_4, DIAGONAL_14, DIAGONAL_18}}},

        // Linha 3
        {10,
         {{5, 6, 11, 15, 16},
          {HORIZONTAL_3, VERTICAL_1, DIAGONAL_15, DIAGONAL_19}}},
        {11,
         {{5, 6, 7, 10, 12, 15, 16, 17},
          {HORIZONTAL_3, VERTICAL_2, VERTICAL_3, DIAGONAL_15, DIAGONAL_16,
           DIAGONAL_19, DIAGONAL_20}}},
        {12,
         {{6, 7, 8, 11, 13, 16, 17, 18},
          {HORIZONTAL_3, VERTICAL_3, VERTICAL_4, DIAGONAL_16, DIAGONAL_17,
           DIAGONAL_20, DIAGONAL_21}}},
        {13,
         {{7, 8, 9, 12, 14, 17, 18, 19},
          {HORIZONTAL_3, VERTICAL_4, VERTICAL_5, DIAGONAL_17, DIAGONAL_18,
           DIAGONAL_21, DIAGONAL_22}}},
        {14,
         {{8, 9, 13, 18, 19},
          {HORIZONTAL_3, VERTICAL_5, DIAGONAL_18, DIAGONAL_22}}},

        // Linha 4
        {15,
         {{10, 11, 16, 20, 21},
          {HORIZONTAL_4, VERTICAL_1, DIAGONAL_19, DIAGONAL_23}}},
        {16,
         {{10, 11, 12, 15, 17, 20, 21, 22},
          {HORIZONTAL_4, VERTICAL_2, VERTICAL_3, DIAGONAL_19, DIAGONAL_20,
           DIAGONAL_23, DIAGONAL_24}}},
        {17,
         {{11, 12, 13, 16, 18, 21, 22, 23},
          {HORIZONTAL_4, VERTICAL_3, VERTICAL_4, DIAGONAL_20, DIAGONAL_21,
           DIAGONAL_24, DIAGONAL_25}}},
        {18,
         {{12, 13, 14, 17, 19, 22, 23, 24},
          {HORIZONTAL_4, VERTICAL_4, VERTICAL_5, DIAGONAL_21, DIAGONAL_22,
           DIAGONAL_24, DIAGONAL_25}}},
        {19,
         {{13, 14, 18, 23, 24},
          {HORIZONTAL_4, VERTICAL_5, DIAGONAL_22, DIAGONAL_25}}},

        // Linha 5
        {20, {{15, 16, 21}, {HORIZONTAL_5, VERTICAL_1, DIAGONAL_23}}},
        {21,
         {{15, 16, 17, 20, 22},
          {HORIZONTAL_5, VERTICAL_2, VERTICAL_3, DIAGONAL_23, DIAGONAL_24}}},
        {22,
         {{16, 17, 18, 21, 23, 26},
          {HORIZONTAL_5, VERTICAL_3, VERTICAL_4, DIAGONAL_24, DIAGONAL_25}}},
        {23,
         {{17, 18, 19, 22, 24},
          {HORIZONTAL_5, VERTICAL_4, VERTICAL_5, DIAGONAL_25}}},
        {24, {{18, 19, 23}, {HORIZONTAL_5, VERTICAL_5, DIAGONAL_23}}},

        // Linha 6 (Triângulo central)
        {26, {{22, 27, 30, 32}, {VERTICAL_3, DIAGONAL_20, DIAGONAL_24}}},
        {27, {{26, 28, 32}, {VERTICAL_3, DIAGONAL_21, DIAGONAL_25}}},
        {28, {{27, 32, 34}, {VERTICAL_3, DIAGONAL_22}}},

        // Linha 7 (base do triângulo)
        {30, {{26, 32}, {VERTICAL_1, DIAGONAL_23}}},
        {32,
         {{26, 27, 28, 30, 34},
          {VERTICAL_3, DIAGONAL_20, DIAGONAL_21, DIAGONAL_22}}},
        {34, {{28, 32}, {VERTICAL_5, DIAGONAL_22}}}};

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
using State = std::array<Symbol, kGridDimension>;  // 0 to kGridDimension - 1

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
    int cell_index_origin;
    int cell_index_destination;

    Action()
        : player_symbol(Symbol::kEmpty),
          cell_index_origin(-1),
          cell_index_destination(-1) {}

    Action(Symbol player_symbol, int cell_origin, int cell_destination)
        : player_symbol(player_symbol),
          cell_index_origin(cell_origin),
          cell_index_destination(cell_destination) {};
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

    Symbol playerToMove =
        Symbol::kC;  // A ver, n sei mt bem como controlar qm ta jogando

    Player GetPlayerToMove(const State& state) const override;

    std::vector<Action> GetActions(const State& state) const override;
    std::vector<Action> GetPlayerActions(const State& state,
                                         Player player) const;
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

    std::optional<int> FindMiddlePosition(int position1, int position3) const;
    bool IsNeighbor(int position1, int position2) const;
    std::vector<int> FindCommonConnections(int position1, int position2) const;
    bool IsAligned(int position1, int position2, int position3,
                   std::vector<int> common_lines) const;
    void AddIndirectNeighbors(const State& state, Player player,
                              std::vector<Action>& actions,
                              int original_position,
                              int current_position) const;

   private:
    // return the intial board
    static State CreateInitialBoard() {
        State board;
        board.fill(Symbol::kEmpty);

        // add blocked cells
        board[kBoardWidth * 5 + 0] = Symbol::kBlock;
        board[kBoardWidth * 5 + 4] = Symbol::kBlock;
        board[kBoardWidth * 6 + 1] = Symbol::kBlock;
        board[kBoardWidth * 6 + 3] = Symbol::kBlock;

        // add animals
        for (int i{0}; i < 3; ++i)
            for (int j{0}; j < 5; ++j) board[kBoardWidth * i + j] = Symbol::kC;
        board[kBoardWidth * 2 + 2] = Symbol::kO;

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
