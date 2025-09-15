#include "sliding_tile_problem.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>

using namespace sliding_tile;

bool SlidingTileProblem::IsSolvable(State const& state) const {
    std::vector<uint64_t> flat_state;
    for (const auto& row : state) {
        for (const auto& tile : row) {
            if (tile != BLANK_TILE)  // Exclude blank tile
                flat_state.push_back(tile);
        }
    }

    uint64_t inversions = 0;
    for (uint64_t i = 0; i < flat_state.size(); ++i) {
        uint64_t num = flat_state[i];
        for (uint64_t j = i + 1; j < flat_state.size(); ++j)
            if (flat_state[j] < num) inversions++;
    }

    bool even_board = (dimension_ % 2 == 0);
    bool even_inversions = (inversions % 2 == 0);

    if (even_board)
        throw std::logic_error(
            "IsSolvable: Even dimension case not implemented");

    // If inversions is even and dimension is odd -> solvable
    if (even_inversions)
        return true;
    else
        return false;
}

State SlidingTileProblem::RandomizeBoard() {
    srand(time(NULL));

    State state = State(dimension_, std::vector<uint64_t>(dimension_, 0));

    uint64_t num_tiles = dimension_ * dimension_;

    // Fill vector with tile numbers 0 to num_tiles-1
    std::vector<uint64_t> tiles(num_tiles);
    std::iota(tiles.begin(), tiles.end(), 0);

    do {
        std::vector<uint64_t> tiles_copy = tiles;
        // Attribute random values to each tile
        for (uint64_t i = 0; i < num_tiles; ++i) {
            int row = i / dimension_;
            int col = i % dimension_;

            // Get random index from remaining tiles
            int rand_index = rand() % (num_tiles - i);
            state[row][col] = tiles_copy[rand_index];
            tiles_copy.erase(tiles_copy.begin() + rand_index);
        }
    } while (!IsSolvable(state));

    return state;
}

std::unique_ptr<State> SlidingTileProblem::GetResult(
    const State& state, const Action& action) const {
    auto new_state = std::make_unique<State>(state);

    // BUG some conversions issues here with int and uint64_t
    // Find blank tile position
    int blank_row = -1;
    int blank_col = -1;
    for (uint64_t i = 0; i < dimension_; ++i) {
        for (uint64_t j = 0; j < dimension_; ++j) {
            if (state[i][j] == BLANK_TILE) {
                blank_row = i;
                blank_col = j;
                break;
            }
        }
        if (blank_row != -1) break;  // Exit outer loop if found
    }

    // Determine new position based on action
    int new_row = blank_row;
    int new_col = blank_col;

    switch (action) {
        case Action::kUp:
            new_row = blank_row - 1;
            break;
        case Action::kDown:
            new_row = blank_row + 1;
            break;
        case Action::kLeft:
            new_col = blank_col - 1;
            break;
        case Action::kRight:
            new_col = blank_col + 1;
            break;
        default:
            throw std::invalid_argument("Invalid action");
    }

    // Check if new position is within bounds
    if (new_row < 0 || new_row >= static_cast<int>(dimension_) || new_col < 0 ||
        new_col >= static_cast<int>(dimension_)) {
        return nullptr;  // Invalid move, return nullptr
    }

    // Swap blank tile with the adjacent tile
    std::swap((*new_state)[blank_row][blank_col],
              (*new_state)[new_row][new_col]);

    return new_state;
}

std::vector<Action> SlidingTileProblem::GetActions(const State& state) const {
    std::vector<Action> actions;

    // Find blank tile position
    int blank_row, blank_col;
    std::tie(blank_row, blank_col) = GetBlankTileIndex(state);

    // Check possible moves
    if (blank_row > 0) actions.push_back(Action::kUp);
    if (blank_row < static_cast<int>(dimension_) - 1)
        actions.push_back(Action::kDown);
    if (blank_col > 0) actions.push_back(Action::kLeft);
    if (blank_col < static_cast<int>(dimension_) - 1)
        actions.push_back(Action::kRight);

    return actions;
}

double SlidingTileProblem::Heuristic(const State& state) const {
    // Using Manhattan distance as heuristic
    double total_distance = 0.0;

    // Find blank tile position
    int blank_row, blank_col;
    std::tie(blank_row, blank_col) = GetBlankTileIndex(state);

    for (uint64_t row = 0; row <= blank_row; ++row) {
        for (uint64_t col = 0; col < blank_col; ++col) {
            uint64_t tile = state[row][col];
            // Calculate goal position
            uint64_t goal_row = tile / dimension_;
            uint64_t goal_col = tile % dimension_;

            // Calculate Manhattan distance
            total_distance +=
                std::abs(static_cast<int>(row) - static_cast<int>(goal_row)) +
                std::abs(static_cast<int>(col) - static_cast<int>(goal_col));
        }
    }

    for (uint64_t row = blank_row; row < dimension_; ++row) {
        for (uint64_t col = blank_col + 1; col < dimension_; ++col) {
            uint64_t tile = state[row][col];
            // Calculate goal position
            uint64_t goal_row = tile / dimension_;
            uint64_t goal_col = tile % dimension_;

            // Calculate Manhattan distance
            total_distance +=
                std::abs(static_cast<int>(row) - static_cast<int>(goal_row)) +
                std::abs(static_cast<int>(col) - static_cast<int>(goal_col));
        }
    }

    return total_distance;
}

// Return blank tile position as (row, col)
std::pair<int, int> SlidingTileProblem::GetBlankTileIndex(
    const State& state) const {
    for (int row = 0; row < (int)dimension_; ++row) {
        for (int col = 0; col < (int)dimension_; ++col) {
            if (state[row][col] == BLANK_TILE) {
                return std::make_pair(row, col);
            }
        }
    }
    return std::make_pair(-1, -1);  // Not found
}

void SlidingTileProblem::PrintState(const State& state) const {
    // Calculate the maximum number (dimension * dimension - 1)
    int max_number = static_cast<int>(dimension_ * dimension_ - 1);

    // Calculate the width needed (number of digits in max_number)
    int width = 0;
    int temp = max_number;
    if (temp == 0) {
        width = 1;
    } else {
        while (temp > 0) {
            width++;
            temp /= 10;
        }
    }

    // Print the state with justified formatting
    for (size_t i = 0; i < state.size(); ++i) {
        for (size_t j = 0; j < state[i].size(); ++j) {
            // Use setw to set field width and right-align numbers
            std::cout << std::setw(width) << state[i][j];

            // Add space between numbers except for the last column
            if (j < state[i].size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

State SlidingTileProblem::GenerateGoalState() const {
    State goal = State(dimension_, std::vector<uint64_t>(dimension_, 0));
    uint64_t count = 0;
    for (uint64_t i = 0; i < dimension_; ++i) {
        for (uint64_t j = 0; j < dimension_; ++j) {
            goal[i][j] = count;
            count++;
        }
    }
    goal[0][0] = BLANK_TILE;  // Blank tile
    return goal;
}