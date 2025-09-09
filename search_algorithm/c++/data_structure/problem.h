#ifndef __PROBLEM_H__
#define __PROBLEM_H__

#include <vector>

template <typename TState, typename TAction>
class Problem {
   protected:
    TState initial_state_;

   public:
    virtual ~Problem() = default;

    // Goal test
    virtual bool IsGoal(const TState& state) const = 0;

    virtual std::vector<TAction> GetActions(const TState& state) const = 0;

    // Transition model, return new state after applying action or nullptr if
    // action is invalid
    virtual std::unique_ptr<TState> GetResult(const TState& state,
                                              const TAction& action) const = 0;

    virtual double GetActionCost(const TState& state, const TAction& action,
                                 const TState& new_state) const = 0;

    virtual TState GetInitialState() const { return initial_state_; }
};

#endif  // __PROBLEM_H__