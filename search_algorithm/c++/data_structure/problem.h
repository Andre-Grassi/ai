#ifndef __PROBLEM_H__
#define __PROBLEM_H__

template <typename TState, typename TAction>
class Problem {
   protected:
    TState initial_state_;

   public:
    virtual ~Problem() = default;

    virtual TState GetInitialState() const = 0;

    // Goal test
    virtual bool IsGoal(const TState& state) const = 0;

    virtual std::vector<TAction> GetActions(const TState& state) const = 0;

    // Transition model
    virtual TState GetResult(const TState& state,
                             const TAction& action) const = 0;

    virtual double GetActionCost(const TState& state, const TAction& action,
                                 const TState& new_state) const = 0;

    virtual TState GetInitialState() const { return initial_state_; }
};

#endif  // __PROBLEM_H__