#ifndef SLIDING_TILE_PROBLEM_H
#define SLIDING_TILE_PROBLEM_H

#include <vector>

#include "../node.h"
#include "../problem.h"

#define BLANK_TILE 0

namespace sliding_tile {

enum class Action { kUp, kDown, kLeft, kRight };

using State = std::vector<std::vector<uint64_t>>;  // 2D grid representation

class SlidingTileProblem : public Problem<State, Action> {
   private:
    State initial_state_;
    uint64_t dimension_ = 3;  // Default to 3x3 board
    const std::vector<std::vector<uint64_t>> goal_state_ = {
        {BLANK_TILE, 1, 2}, {3, 4, 5}, {6, 7, 8}};  // Goal state

    // Return random solvable 3x3 board
    State RandomizeBoard();

    bool IsSolvable(const State& state) const;

   public:
    SlidingTileProblem(const State& initial_state, const uint64_t dimension)
        : initial_state_(initial_state), dimension_(dimension) {}

    SlidingTileProblem(const uint64_t dimension) {
        dimension_ = dimension;
        initial_state_ = RandomizeBoard();
    }

    virtual ~SlidingTileProblem() = default;

    virtual State GetInitialState() const override { return initial_state_; }

    virtual bool IsGoal(const State& state) const override {
        return state == goal_state_;
    }

    virtual std::vector<Action> GetActions(const State& state) const override;

    virtual std::unique_ptr<State> GetResult(
        const State& state, const Action& action) const override;

    virtual double GetActionCost(const State&, const Action&,
                                 const State&) const override {
        return 1.0;  // Uniform cost for all actions
    }

    uint64_t GetDimension() const { return dimension_; }

    // Return blank tile position as (row, col)
    // (-1, -1) if not found
    std::pair<int, int> GetBlankTileIndex(const State& state) const;

    void PrintState(const State& state) const;
};

}  // namespace sliding_tile

#endif  // SLIDING_TILE_PROBLEM_H