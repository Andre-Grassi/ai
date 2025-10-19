#include "chess_board_problem.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <queue>
#include <limits>

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
    (*new_state)[row][col] = Piece::EMPTY;  // Empty origin cell

    // Special case: pawn promotion
    if (piece == Piece::PAWN &&
        toRow == 1)  // hardcoded toRow 1 makes the pawn a queen
        (*new_state)[row][col] = Piece::QUEEN;
    else
        (*new_state)[toRow][toCol] = piece;

    return new_state;
}

std::vector<Action> ChessBoardProblem::GetActions(const State& state) const {
    std::vector<Action> actions;

    int num_rows = state.size();
    int num_cols = state[0].size();

    for (int row = 0; row < num_rows; ++row)
        for (int col = 0; col < num_cols; ++col) {
            Piece piece_to_move = static_cast<Piece>(state[row][col]);

            // Map all possible moves
            switch (piece_to_move) {
                case Piece::WHITE_KNIGHT:
                case Piece::BLACK_KNIGHT: {
                    const int row_move[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
                    const int col_move[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
                    for (int i{0}; i < 8; ++i) {
                        int dest_row = row + row_move[i];
                        int dest_col = col + col_move[i];
                        if (dest_row >= 0 && dest_row < num_rows &&
                            dest_col >= 0 && dest_col < num_cols &&
                            state[dest_row][dest_col] == Piece::EMPTY)
                            actions.emplace_back(piece_to_move, row, col,
                                                 dest_row, dest_col);
                    }
                    break;
                }
                case Piece::ROOK: {
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

                            actions.emplace_back(piece_to_move, row, col,
                                                 dest_row, dest_col);
                        }
                    }
                    break;
                }
                case Piece::BISHOP: {
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

                            actions.emplace_back(piece_to_move, row, col,
                                                 dest_row, dest_col);
                        }
                    }
                    break;
                }
                case Piece::QUEEN: {
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

                            actions.emplace_back(piece_to_move, row, col,
                                                 dest_row, dest_col);
                        }
                    }
                    break;
                }
                case Piece::PAWN: {
                    int dest_row = row - 1;  // pawn moving up
                    if (dest_row >= 0 && dest_row < num_rows)
                        if (state[dest_row][col] == Piece::EMPTY)
                            actions.emplace_back(piece_to_move, row, col,
                                                 dest_row, col);
                    break;
                }

                case Piece::EMPTY:
                case Piece::BORDER:
                case Piece::ANY:
                    break;  // No moves for these pieces
            }
        }

    return actions;
}

auto
knight_next_jump (int knight_r, int knight_c) -> std::vector<std::pair<int, int>> {
    // Deslocamentos fixos do cavalo (dr, dc)
    std::vector<std::pair<int, int>> moves = {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        { 1, -2}, { 1, 2},
        { 2, -1}, { 2, 1}
    };

    std::vector<std::pair<int, int>> possible_squares;

    for (const auto& move : moves){
        int new_r = knight_r + move.first;
        int new_c = knight_c + move.second;

        if (new_r >= 0 && new_r < BOARDDIMENSION &&
            new_c >= 0 && new_c < BOARDDIMENSION)
        {
            possible_squares.push_back({new_r, new_c});
        }
    }

    return possible_squares;
}

std::vector<std::vector<ChessCostType>> KnightLookupTable(int goal_r, int goal_c) {
    const ChessCostType UNVISITED = std::numeric_limits<ChessCostType>::max();
    std::vector<std::vector<ChessCostType>> LookupTable
                    (BOARDDIMENSION, std ::vector<ChessCostType>(BOARDDIMENSION, UNVISITED));
    
    std::pair<int, int> coordinates;
    std::vector<std::pair<int, int>> next_squares;
    
    std::queue<std::pair<int, int>> tree;
    
    ChessCostType current_value;

    LookupTable[goal_r][goal_c] = static_cast<ChessCostType>(0.0);
    tree.push({goal_r, goal_c});
    
    while (!tree.empty()){
        std::pair<int, int> coordinates = tree.front();
        tree.pop();

        current_value = LookupTable[coordinates.first][coordinates.second];
        next_squares = knight_next_jump(coordinates.first, coordinates.second);
        
        for (const auto& square : next_squares){
            if (LookupTable[square.first][square.second] == UNVISITED) {
                LookupTable[square.first][square.second] = current_value + 1.0;
                tree.push(square);
            }
        }
    }

    return LookupTable;
}

ChessCostType ChessBoardProblem::Heuristic(const State& state) const {
    const int rows = static_cast<int>(state.size());
    const int cols = rows ? static_cast<int>(state[0].size()) : 0;

    if (rows == 0 || cols == 0) return static_cast<ChessCostType>(0.0);
    if (IsGoal(state)) return static_cast<ChessCostType>(0.0);

    const ChessCostType UNVISITED = std::numeric_limits<ChessCostType>::max();

    // Identify which problem it is based on preset_state_
    bool isProblem1 = (preset_state_ == 1);
    bool isProblem2 = (preset_state_ == 2);

    // Admissible heuristic for Problem 1: Black knight at (3,6)
    if (isProblem1) {
        auto [knight_r, knight_c] =
            FindPiecePosition(state, Piece::BLACK_KNIGHT);

        if (knight_r == -1) return static_cast<ChessCostType>(0.0);

        if (knight_r >= 0 && knight_r < BOARDDIMENSION &&
            knight_c >= 0 && knight_c < BOARDDIMENSION)
            return LookupTable[knight_r][knight_c];
            
        return static_cast<ChessCostType>(0.0);
    }

    // Admissible heuristic for Problem 2: Pawn → Queen → (4,1)
    if (isProblem2) {
        const int promotion_row = 1;
        const int queen_goal_r = 4, queen_goal_c = 1;

        auto [queen_r, queen_c] = FindPiecePosition(state, Piece::QUEEN);
        auto [pawn_r, pawn_c] = FindPiecePosition(state, Piece::PAWN);

        // Phase 2: Queen exists
        if (queen_r != -1) {
            int dr = abs(queen_r - queen_goal_r);
            int dc = abs(queen_c - queen_goal_c);

            // Admissible heuristic for Queen (Chebyshev distance)
            ChessCostType h = static_cast<ChessCostType>(std::max(dr, dc));
            return h;
        }
        // Phase 1: Pawn exists
        else if (pawn_r != -1) {
            // Minimum cost for pawn to reach promotion row
            // This is a guaranteed underestimate
            ChessCostType pawn_cost =
                static_cast<ChessCostType>(pawn_r - promotion_row);

            // Absolute minimum cost for queen (Chebyshev distance)
            int dr = abs(promotion_row - queen_goal_r);
            int dc = abs(pawn_c - queen_goal_c);
            ChessCostType queen_cost =
                static_cast<ChessCostType>(std::max(dr, dc));

            // Sum of admissible heuristics is admissible
            return pawn_cost + queen_cost;
        }

        return static_cast<ChessCostType>(0.0);
    }

    // Default case: admissible heuristic, but not informative
    return static_cast<ChessCostType>(0.0);
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

void ChessBoardProblem::PrintAction(const Action& action) const {
    std::cout << (char)action.piece
              << " " + std::to_string(action.fromRow) + " "
              << std::to_string(action.fromCol) + " "
              << std::to_string(action.toRow) + " "
              << std::to_string(action.toCol);
}

std::pair<int, int> ChessBoardProblem::FindPiecePosition(
    const State& state, Piece piece_to_find) const {
    for (size_t r = 0; r < state.size(); ++r) {
        for (size_t c = 0; c < state[r].size(); ++c) {
            if (static_cast<Piece>(state[r][c]) == piece_to_find) {
                return {static_cast<int>(r),
                        static_cast<int>(c)};  // Piece found
            }
        }
    }
    return {-1, -1};  // Piece not found
}