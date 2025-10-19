#ifndef SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_CHESS_BOARD_H_
#define SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_CHESS_BOARD_H_

#include <vector>

#include "../node.h"
#include "../problem.h"

#define BOARDDIMENSION 8

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
    // preset 1 ou 2 pra definir qual dos 2 exemplos do enunciado usar,
    // 0 usa um tabuleiro aleatorio resolvivel --nao implementado, dificil
    State GenerateInitialState(const int preset_state) const;
    State GenerateGoalState(const int preset_state) const;

    State goal_state_;

    int preset_state_;  // 1 para Problema1, 2 para Problema2, etc.

    private:
        // Função auxiliar para encontrar a posição de uma peça específica
        std::pair<int, int> FindPiecePosition(const State& state, Piece piece_to_find) const {
            for (int r = 0; r < state.size(); ++r) {
                for (int c = 0; c < state[r].size(); ++c) {
                    if (state[r][c] == piece_to_find) {
                        return {r, c};
                    }
                }
            }
            return {-1, -1}; // Peça não encontrada
        }
   public:
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

    std::vector<std::vector<ChessCostType>> KnightLookupTable(int goal_r, int goal_c);

    ChessCostType Heuristic(const State& state) const override;

    State GetGoalState() const { return goal_state_; }

    void PrintState(const State& state) const;
};

}  // namespace chess_board

#endif
