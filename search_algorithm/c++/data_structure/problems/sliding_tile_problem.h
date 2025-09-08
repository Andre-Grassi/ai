#include <vector>

#include "../node.h"
#include "../problem.h"

namespace sliding_tile {

enum class Action { kUp, kDown, kLeft, kRight };

using State = std::vector<std::vector<int>>;  // 2D grid representation

class SlidingTileProblem : public Problem<State, Action> {
   public:
    SlidingTileProblem(const State& initial_state)
        : initial_state_(initial_state) {}

    virtual ~SlidingTileProblem() = default;

    virtual State GetInitialState() const override;

    virtual bool IsGoal(const State& state) const override;

    virtual std::vector<Action> GetActions(const State& state) const override;

    virtual State GetResult(const State& state,
                            const Action& action) const override;

    virtual double GetActionCost(const State& state, const Action& action,
                                 const State& new_state) const override;

   private:
    State initial_state_;
};

}  // namespace sliding_tile