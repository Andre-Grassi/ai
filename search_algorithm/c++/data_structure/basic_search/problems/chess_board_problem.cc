#include "chess_board_problem.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>

using namespace chess_board;

State ChessBoardProblem::GenerateInitialState(const int preset_state) const {
    State s;
    switch (preset_state) {
        case 1:
            s = State(5, std::vector<uint64_t>(8, '#'));
            for (int i{1}; i <= 4; ++i) {
                s[2][i] = Piece::WHITE_KNIGHT;
                s[1][i + 1] = Piece::BISHOP;
            }

            s[1][1] = Piece::BLACK_KNIGHT;
            s[1][6] = Piece::ROOK;
            s[2][5] = Piece::ROOK;
            s[2][6] = Piece::ROOK;
            s[3][5] = Piece::ROOK;
            s[3][6] = Piece::EMPTY;
            return s;

        case 2:
            s = State(6, std::vector<uint64_t>(6, '#'));
            for (int i{1}; i <= 4; ++i) {
                s[1][i] = Piece::WHITE_KNIGHT;
                s[2][i] = Piece::BISHOP;
                s[3][i] = Piece::ROOK;
            }

            s[4][1] = Piece::EMPTY;
            s[4][4] = Piece::PAWN;
            return s;

        default:
            throw std::logic_error(
                "GenerateInitialState:\
                Random initial board case not implemented, use preset 1 or 2");
    }
}

std::unique_ptr<State> ChessBoardProblem::GetResult(
    const State& state, const Action& action) const {
    auto new_state = std::make_unique<State>(state);

    // simplificar
    int row = action.fromRow;
    int col = action.fromCol;
    int toRow = action.toRow;
    int toCol = action.toCol;

    Piece piece = static_cast<Piece>((*new_state)[row][col]);
    (*new_state)[row][col] = Piece::EMPTY;  // origem fica vazia

    // Caso especial: promoção de peão
    if (piece == Piece::PAWN &&
        toRow == 1)  // hardcoded toRow 1 faz virar rainha
        (*new_state)[row][col] = Piece::QUEEN;
    else
        (*new_state)[toRow][toCol] = piece;

    return new_state;
}

// retorna todos os movimentos possiveis em coordenadas (x, y, dx, dy)
std::vector<Action> ChessBoardProblem::GetActions(const State& state) const {
    std::vector<Action> actions;

    int num_rows = state.size();
    int num_cols = state[0].size();

    for (int row = 0; row < num_rows; ++row)
        for (int col = 0; col < num_cols; ++col) {
            int64_t cell = state[row][col];
            if (cell == Piece::BORDER || cell == EMPTY) continue;  // vazio

            // mapeia todos os movimentos possíveis
            switch (cell) {
                case WHITE_KNIGHT:
                case BLACK_KNIGHT: {
                    const int row_move[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
                    const int col_move[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
                    for (int i{0}; i < 8; ++i) {
                        int dest_row = row + row_move[i];
                        int dest_col = col + col_move[i];
                        if (dest_row >= 0 && dest_row < num_rows &&
                            dest_col >= 0 && dest_col < num_cols &&
                            state[dest_row][dest_col] == Piece::EMPTY)
                            actions.emplace_back(row, col, dest_row, dest_col);
                    }
                    break;
                }
                case ROOK: {
                    const int row_move[4] = {-1, 1, 0, 0};
                    const int col_move[4] = {0, 0, -1, 1};
                    for (int i = 0; i < 4; ++i) {
                        int dest_row = row, dest_col = col;
                        while (true) {
                            dest_row += row_move[i];
                            dest_col += col_move[i];
                            if (dest_row < 0 || dest_row >= num_rows ||
                                dest_col < 0 || dest_col >= num_cols ||
                                state[dest_row][dest_col] != EMPTY)
                                break;

                            actions.emplace_back(row, col, dest_row, dest_col);
                        }
                    }
                    break;
                }
                case BISHOP: {
                    const int row_move[4] = {-1, -1, 1, 1};
                    const int col_move[4] = {-1, 1, -1, 1};
                    for (int i = 0; i < 4; ++i) {
                        int dest_row = row;
                        int dest_col = col;
                        while (true) {
                            dest_row += row_move[i];
                            dest_col += col_move[i];
                            if (dest_row < 0 || dest_row >= num_rows ||
                                dest_col < 0 || dest_col >= num_cols ||
                                state[dest_row][dest_col] != Piece::EMPTY)
                                break;

                            actions.emplace_back(row, col, dest_row, dest_col);
                        }
                    }
                    break;
                }
                case QUEEN: {
                    const int row_move[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
                    const int col_move[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
                    for (int i = 0; i < 8; ++i) {
                        int dest_row = row;
                        int dest_col = col;
                        while (true) {
                            dest_row += row_move[i];
                            dest_col += col_move[i];
                            if (dest_row < 0 || dest_row >= num_rows ||
                                dest_col < 0 || dest_col >= num_cols ||
                                state[dest_row][dest_col] != Piece::EMPTY)
                                break;

                            actions.emplace_back(row, col, dest_row, dest_col);
                        }
                    }
                    break;
                }
                case PAWN: {
                    int dest_row = row - 1;  // peao andando pra cima
                    if (dest_row >= 0 && dest_row < num_rows)
                        if (state[dest_row][col] == Piece::EMPTY)
                            actions.emplace_back(row, col, dest_row, col);
                    break;
                }
            }
        }

    return actions;
}

ChessCostType ChessBoardProblem::Heuristic(const State& state) const {
    return 0;
}

void ChessBoardProblem::PrintState(const State& state) const {
    for (auto r : state) {
        for (auto v : r) std::cout << static_cast<char>(v);
        std::cout << '\n';
    }
    std::cout << std::endl;
}

bool ChessBoardProblem::IsGoal(const State& state) const {
    int rows = goal_state_.size();
    int cols = goal_state_[0].size();

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            auto goal_cell = goal_state_[r][c];
            auto curr_cell = state[r][c];

            if (goal_cell == Piece::ANY) continue;  // ignora '?'
            if (curr_cell != goal_cell) return false;
        }
    }

    return true;
}

State ChessBoardProblem::GenerateGoalState(const int preset_state) const {
    State s;
    switch (preset_state) {
        case 1:
            s = State(5, std::vector<uint64_t>(8, '#'));
            for (int i{1}; i <= 2; ++i)
                for (int j{1}; j <= 6; ++j) s[i][j] = Piece::ANY;

            s[3][5] = Piece::ANY;
            s[3][6] = Piece::BLACK_KNIGHT;
            return s;

        case 2:
            s = State(6, std::vector<uint64_t>(6, '#'));
            for (int i{1}; i <= 3; ++i)
                for (int j{1}; j <= 4; ++j) s[i][j] = Piece::ANY;

            s[4][4] = Piece::ANY;
            s[4][1] = Piece::QUEEN;
            return s;

        default:
            throw std::logic_error(
                "GenerateInitialState:\
                Random initial board case not implemented, use preset 1 or 2");
    }
}
