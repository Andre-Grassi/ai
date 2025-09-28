#ifndef SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_CHESS_BOARD_H_
#define SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_CHESS_BOARD_H_

#include <vector>

#include "data_structure/node.h"
#include "data_structure/problem.h"

namespace chess_board {

/**
 * @brief Available chess pieces represented as integer values
 * @note Only a subset of pieces are implemented for this problem
 */
enum Piece : int64_t {
    EMPTY = '_',
    BORDER = '#',
    ANY = '?',
    ROOK = 't',
    PAWN = 'p',
    QUEEN = 'q',
    BISHOP = 'b',
    WHITE_KNIGHT = 'c',
    BLACK_KNIGHT = 'k',
};

/**
 * @brief Action representation for moving a piece on the chess board
 *
 * An action consists of moving a specific piece from one position
 * (fromRow, fromCol) to another position (toRow, toCol).
 */
struct Action {
   public:
    Action() = default;
    Action(Piece piece, int fromRow, int fromCol, int toRow, int toCol)
        : piece(piece),
          fromRow(fromRow),
          fromCol(fromCol),
          toRow(toRow),
          toCol(toCol) {}
          
    Piece piece;  /// < Used only to show which piece was moved when printing
                  /// actions
    int fromRow, fromCol, toRow, toCol;
};

using State = std::vector<std::vector<uint64_t>>;  /// < 2D grid representation

using ChessCostType =
    float;  ///< Cost type for actions and to calculate heuristics

/**
 * @brief Chess path finding puzzle problem implementation
 * @tparam State State representation as a 2D grid of pieces
 * @tparam Action Action representation for moving pieces
 * @tparam ChessCostType Cost type for actions and heuristics
 */
class ChessBoardProblem : public Problem<State, Action, ChessCostType> {
   public:
    /**
     * @brief Constructs the problem with specified problem identifier
     * @param preset_state Preset configuration identifier:
     *                     - 1: Use Problem1 configuration from assignment
     *                     - 2: Use Problem2 configuration from assignment
     *                     - 0: Use random solvable board (not implemented)
     */
    ChessBoardProblem(int preset_state = 0)
        : Problem<State, Action, ChessCostType>(
              GenerateInitialState(preset_state)),
          goal_state_(GenerateGoalState(preset_state)),
          preset_state_(preset_state) {}

    virtual ~ChessBoardProblem() = default;

    virtual bool IsGoal(const State& state) const override;

    virtual std::vector<Action> GetActions(const State& state) const override;

    virtual std::unique_ptr<State> GetResult(
        const State& state, const Action& action) const override;

    virtual ChessCostType GetActionCost(const State&, const Action&,
                                        const State&) const {
        return 1.0;  // Uniform cost for all actions
    }

    ChessCostType Heuristic(const State& state) const override;

    State GetGoalState() const { return goal_state_; }

    void PrintState(const State& state) const;

    /** @brief Prints the details of an action
     *
     * Print as follows: "<piece> <fromRow> <fromCol> <toRow> <toCol>"
     *
     * @param action The action to print
     */
    void PrintAction(const Action& action) const;

   private:
    State goal_state_;  /// < Target configuration to reach
    int preset_state_;  /// < Identifier of the statement problem (1 or 2)

    /**
     * @brief Generates the initial state based on preset configuration
     * @param preset_state Preset configuration identifier:
     *                     - 1: Use Problem1 configuration from assignment
     *                     - 2: Use Problem2 configuration from assignment
     *                     - 0: Use random solvable board (not implemented)
     * @return The initial state configuration
     */
    State GenerateInitialState(const int preset_state) const;

    /**
     * @brief Generates the goal state based on preset configuration
     * @param preset_state Identifier of the statement problem (1 or 2)
     * @return The goal state configuration
     */
    State GenerateGoalState(const int preset_state) const;

    /**
     * @brief Auxiliary function to find the position of a specific piece on the
     * board
     * @param state The current board state to search
     * @param piece_to_find The piece to locate on the board
     * @return std::pair<int, int> The (row, col) position of the piece, or (-1,
     * -1) if not found
     */
    std::pair<int, int> FindPiecePosition(const State& state,
                                          Piece piece_to_find) const;
};

}  // namespace chess_board

#endif
