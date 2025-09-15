#ifndef SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_SLIDING_H_
#define SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_SLIDING_H_

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
    State goal_state_;

    // Return random solvable 3x3 board
    State RandomizeBoard();

    bool IsSolvable(const State& state) const;

    State GenerateGoalState() const;

   public:
    SlidingTileProblem(const State& initial_state, const uint64_t dimension)
        : initial_state_(initial_state),
          dimension_(dimension),
          goal_state_(GenerateGoalState()) {}

    SlidingTileProblem(const uint64_t dimension) {
        dimension_ = dimension;
        initial_state_ = RandomizeBoard();
        goal_state_ = GenerateGoalState();
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

    double Heuristic(const State& state) const override;

    uint64_t GetDimension() const { return dimension_; }

    // Return blank tile position as (row, col)
    // (-1, -1) if not found
    std::pair<int, int> GetBlankTileIndex(const State& state) const;

    State GetGoalState() const { return goal_state_; }

    void PrintState(const State& state) const;
};

}  // namespace sliding_tile

#endif  // SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_SLIDING_H_