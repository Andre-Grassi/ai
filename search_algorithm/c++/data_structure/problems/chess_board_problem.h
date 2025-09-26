#ifndef SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_CHESS_BOARD_H_
#define SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_CHESS_BOARD_H_

#include <vector>

#include "../node.h"
#include "../problem.h"

namespace chess_board {

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
// Peças não utilizadas não foram implementadas

class Action {
   public:
    Action() = default;
    Action(int fromRow, int fromCol, int toRow, int toCol)
        : fromRow(fromRow), fromCol(fromCol), toRow(toRow), toCol(toCol) {}
    int fromRow, fromCol, toRow, toCol;
};

using State = std::vector<std::vector<uint64_t>>;  // 2D grid representation

using ChessCostType = float;  // WARNING: Apenas um placeholder por enquanto

class ChessBoardProblem : public Problem<State, Action, ChessCostType> {
   private:
    State initial_state_;
    State goal_state_;

    // preset 1 ou 2 pra definir qual dos 2 exemplos do enunciado usar,
    // 0 usa um tabuleiro aleatorio resolvivel --nao implementado, dificil
    State GenerateInitialState(const int preset_state) const;
    State GenerateGoalState(const int preset_state) const;

   public:
    ChessBoardProblem(int preset_state = 0)
        : initial_state_(GenerateInitialState(preset_state)),
          goal_state_(GenerateGoalState(preset_state)) {}

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
};

}  // namespace chess_board

#endif
