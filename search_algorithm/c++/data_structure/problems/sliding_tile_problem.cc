#include "sliding_tile_problem.h"

#include <cstdlib>
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

    // If inversions is even and dimension is odd -> solvable
    if (!even_board && even_inversions)
        return true;
    else
        throw std::logic_error(
            "IsSolvable: Even dimension case not implemented");
}

State SlidingTileProblem::RandomizeBoard() {
    State state = State(dimension_, std::vector<uint64_t>(dimension_, 0));

    uint64_t num_tiles = dimension_ * dimension_;

    std::vector<uint64_t> tiles(num_tiles);
    std::iota(tiles.begin(), tiles.end(), 0);

    do {
        srand(time(NULL));

        // Attribute random values to each tile
        for (uint64_t i = 0; i < num_tiles; ++i) {
            int row = i / dimension_;
            int col = i % dimension_;

            // Get random index from remaining tiles
            int rand_index = rand() % (num_tiles - i);
            state[row][col] = tiles[rand_index];
            tiles.erase(tiles.begin() + rand_index);
        }
    } while (!IsSolvable(state));

    return state;
}

State* SlidingTileProblem::GetResult(const State& state,
                                     const Action& action) const {
    State* new_state = new State(state);

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
    std::swap(new_state[blank_row][blank_col], new_state[new_row][new_col]);

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