#ifndef SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_
#define SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_

#define IGNORE_TERMINAL_TEST true

#include <array>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "data_structure/adversarial_search/game.h"

namespace adugo_game {

// ---------- tabela ----------
// formato: node_index -> {neighbors }
const std::map<int, std::vector<int>> kGridDimensionNeighborhood = {

    // Linha 1
    {0, {1, 5, 6}},
    {1, {0, 2, 6}},
    {2, {1, 3, 6, 7, 8}},
    {3, {2, 4, 8}},
    {4, {3, 8, 9}},

    // Linha 2
    {5, {0, 6, 10}},
    {6, {0, 1, 2, 5, 7, 10, 11, 12}},
    {7, {2, 6, 8, 12}},
    {8, {2, 3, 4, 7, 9, 12, 13, 14}},
    {9, {4, 8, 14}},

    // Linha 3
    {10, {5, 6, 11, 15, 16}},
    {11, {6, 10, 12, 16}},
    {12, {6, 7, 8, 11, 13, 16, 17, 18}},
    {13, {8, 12, 14, 18}},
    {14, {8, 9, 13, 18, 19}},

    // Linha 4
    {15, {10, 16, 20}},
    {16, {10, 11, 12, 15, 17, 20, 21, 22}},
    {17, {12, 16, 18, 22}},
    {18, {12, 13, 14, 17, 19, 22, 23, 24}},
    {19, {14, 18, 24}},

    // Linha 5
    {20, {15, 16, 21}},
    {21, {16, 20, 22}},
    {22, {16, 17, 18, 21, 23, 26, 27, 28}},
    {23, {18, 22, 24}},
    {24, {18, 19, 23}},

    // Linha 6 (Triângulo central)
    {26, {22, 27, 30}},
    {27, {22, 26, 28, 32}},
    {28, {22, 27, 34}},

    // Linha 7 (base do triângulo)
    {30, {26, 32}},
    {32, {27, 30, 34}},
    {34, {28, 32}}};

enum class Symbol : char {
    kEmpty = '-',  // Empty cell
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
 * @brief Represent the board configuration as a 1D array.
 * Indexed from * 0 to kGridDimension-1
 * @note Using array instead of vector to define fixed dimension.
 *
 * board layout:
 * 0   1  2  3  4
 * 5   6  7  8  9
 * 10 11 12 13 14
 * 15 16 17 18 19
 * 20 21 22 23 24
 * 25 26 27 28 29
 * 30 31 32 33 34
 */
using Board = std::array<Symbol, kGridDimension>;  // 0 to kGridDimension - 1

/**
 * @brief State of the Adugo game. Stores the board and which player's turn it
 * is.
 */
struct State {
   public:
    Board board;
    Player player_to_move;

    // This board is not playable, used to indicate a default invalid state
    State() : board{}, player_to_move() { board.fill(Symbol::kEmpty); }

    State(Board board_array, Player player_to_move)
        : board(board_array), player_to_move(player_to_move) {}

    /**
     * @brief Access operator. Get the symbol from the board at the given index.
     * @param index Index of the cell to access (0 to kGridDimension-1).
     * @return Reference to the symbol at the specified index.
     */
    Symbol& operator[](int index) { return board[index]; }
    const Symbol& operator[](int index) const { return board[index]; }

    // Used for hashing and comparisons in unordered_map
    bool operator==(const State& other) const {
        return (this->board == other.board);  // Only the board matters
    }

    // Override print
    friend std::ostream& operator<<(std::ostream& os, const State& state) {
        for (size_t i = 0; i < kGridDimension; ++i) {
            os << static_cast<char>(state.board[i]) << ' ';
            if ((i + 1) % kBoardWidth == 0) os << std::endl;
        }
        return os;
    }
};

// Action/Move is a struct that tells where the player is "drawing" its
// symbol
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

    // Override print
    friend std::ostream& operator<<(std::ostream& os, const Action& action) {
        os << "Player: " << static_cast<char>(action.player_symbol)
           << ", From: " << action.cell_index_origin
           << ", To: " << action.cell_index_destination;
        return os;
    }
};

/// WARNING: UTILITY MUST BE FLOAT OR DOUBLE, otherwise the GetEval will be
/// inneficient using integer values
using Utility = float;  // Value from -1 (loss) to +1 (win) from MAX player's
                        // perspective

class AdugoGame;  // forward declaration
};  // namespace adugo_game

// Hash specialization for State to be used in unordered_map
namespace std {
template <>
struct hash<adugo_game::State> {
    std::size_t operator()(const adugo_game::State& state) const {
        size_t hash_value = 0;
        // Hash the board
        for (size_t i = 0; i < state.board.size(); ++i) {
            hash_value ^= std::hash<int>{}(static_cast<int>(state.board[i])) +
                          0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
        }
        return hash_value;
    }
};
}  // namespace std

namespace adugo_game {

/**
 * @brief Class representing the Adugo game. The jaguar is MIN, and the dogs are
 * MAX.
 * State: Struct with the board and an indication of which player turn it is.
 * Action: Player's symbol and cell index (0-30)
 * Utility: -1 (loss), 0 (draw), +1 (win) from MAX player's perspective
 * Player: Struct with symbol and IsMax() method
 */
class AdugoGame : public Game<State, Action, Utility, Player> {
   public:
    static const int kNumStartingDogs = 14;  ///< Number of dogs at game start
    static const int kNumDogsToCapture =
        5;  ///< Number of dogs the jaguar must capture to win
    static const int kMaxDepth = 10;  ///< Default maximum search depth

    std::unordered_map<State, Utility> transposition_table;

    AdugoGame(int max_depth = kMaxDepth)
        : Game(CreateInitialBoard(), max_depth) {}

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

    /**
     * @brief Evaluates the current game state using a heuristic function. If
     * it's terminal state, just return the utility with GetUtility().
     *
     * This function calculates a utility value from the dogs' perspective
     * using:
     *
     * Heuristic = 1.0 - 2.0 * (jaguar_score / max_jaguar_score)
     *
     * Where:
     * - jaguar_score = (captured_dogs * 100) + (jaguar_mobility * 1)
     * - max_jaguar_score = (total_capturable_dogs * 100) + (max_mobility * 1)
     *
     * @param state The current game state to evaluate
     * @return Utility value in range [-1, 1] where:
     *         - 1.0 = best outcome for dogs (jaguar has no captures/mobility)
     *         - -1.0 = worst outcome for dogs (jaguar has maximum
     * captures/mobility)
     *         - 0.0 = neutral/balanced position
     * @warning To work properly, Utility MUST be float or double.
     */
    Utility GetEval(const State& state) const override;

    virtual std::string GetStateString(const State& state) const override;

    std::optional<int> FindMiddlePosition(int position1, int position3) const;
    bool IsNeighbor(int position1, int position2) const;
    std::vector<int> FindCommonConnections(int position1, int position2) const;
    bool IsAligned(int starting_jaguar_pos, int middle_dog_pos,
                   int landing_pos) const;
    void AddIndirectNeighbors(const State& state, Player player,
                              std::vector<Action>& actions,
                              int original_position,
                              int current_position) const;
    int GetJaguarPosition(const State& state) const;
    bool IsCaptureMove(const Action& action) const;


   private:
    /**
     * @brief Alignment types between pieces.
     */
    enum class Alignment { kNotAligned, kHorizontal, kVertical, kDiagonal };

    // return the intial board
    static State CreateInitialBoard() {
        using s = adugo_game::Symbol;
        Board board;
        board = {s::kC,     s::kC,     s::kC,     s::kC,     s::kC,
                 s::kC,     s::kC,     s::kC,     s::kC,     s::kC,
                 s::kC,     s::kC,     s::kO,     s::kC,     s::kC,
                 s::kEmpty, s::kEmpty, s::kEmpty, s::kEmpty, s::kEmpty,
                 s::kEmpty, s::kEmpty, s::kEmpty, s::kEmpty, s::kEmpty,
                 s::kBlock, s::kEmpty, s::kEmpty, s::kEmpty, s::kBlock,
                 s::kEmpty, s::kBlock, s::kEmpty, s::kBlock, s::kEmpty};

        // Starting player is always the jaguar (Onca)
        Player player_to_move(Symbol::kO);

        return State(board, player_to_move);
    }

    /**
     * @return Player that won. If it's a draw or a non terminal state, the
     * player's symbol will be kEmpty.
     */
    Player CalculateWinner(const State& state) const;

    /**
     * @brief Determines the alignment type between two positions on the board.
     * @param pos1 The first position index.
     * @param pos2 The second position index.
     * @return The alignment type (horizontal, vertical, diagonal, or not
     * aligned).
     */
    Alignment GetAlignment(int pos1, int pos2) const;
};
};  // namespace adugo_game

#endif  // SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_SEARCH_GAMES_ADUGO_GAME_H_
