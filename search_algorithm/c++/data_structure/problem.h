/**
 * @file problem.h
 * @brief Abstract base class for search problems
 * @author Andre Grassi
 * @date 2025
 */

#ifndef SEARCH_ALG_DATA_STRUCTURE_PROBLEM_H_
#define SEARCH_ALG_DATA_STRUCTURE_PROBLEM_H_

#include <string>
#include <vector>

/**
 * @brief Abstract base class for search problems
 *
 * This class defines the interface that all search problems must implement.
 * It provides the basic components needed for search algorithms:
 * - Initial state
 * - Goal testing
 * - Action generation
 * - State transitions
 * - Action costs
 * - Heuristic evaluation
 *
 * @tparam TState Type representing a state in the problem space
 * @tparam TAction Type representing actions that can be applied to states
 * @tparam CostType Type representing the cost of actions
 */
template <typename TState, typename TAction, typename CostType>
class Problem {
   protected:
    TState initial_state_;  ///< The starting state of the problem

   public:
    Problem(TState initial_state) : initial_state_(initial_state) {}

    /**
     * @brief Virtual destructor
     *
     * Ensures proper cleanup of derived classes when called through base
     * pointer.
     */
    virtual ~Problem() = default;

    /**
     * @brief Tests if a given state is a goal state
     *
     * This method determines whether the search has reached a solution.
     * Must be implemented by concrete problem classes.
     *
     * @param state The state to test
     * @return true if the state is a goal state, false otherwise
     */
    virtual bool IsGoal(const TState& state) const = 0;

    /**
     * @brief Gets all valid actions available from a given state
     *
     * Returns a list of all actions that can be legally applied to the
     * given state. The list may be empty if no actions are available
     * (dead end state).
     *
     * @param state The state to get actions from
     * @return Vector of valid actions from the given state
     */
    virtual std::vector<TAction> GetActions(const TState& state) const = 0;

    /**
     * @brief Applies an action to a state and returns the resulting state
     *
     * This is the transition model of the problem. It defines how actions
     * transform states. Returns nullptr if the action is invalid for the
     * given state.
     *
     * @param state The current state
     * @param action The action to apply
     * @return Unique pointer to the new state, or nullptr if action is invalid
     */
    virtual std::unique_ptr<TState> GetResult(const TState& state,
                                              const TAction& action) const = 0;

    /**
     * @brief Calculates the cost of applying an action
     *
     * Returns the cost of performing the given action in the given state,
     * resulting in the new state. This is used by cost-based search algorithms
     * like uniform cost search and A*.
     *
     * @param state The current state
     * @param action The action being applied
     * @param new_state The resulting state after applying the action
     * @return The cost of the action as CostType
     */
    virtual CostType GetActionCost(const TState& state, const TAction& action,
                                   const TState& new_state) const = 0;

    /**
     * @brief Gets the initial state of the problem
     *
     * Returns the starting state from which the search begins.
     * This is typically set during problem construction.
     *
     * @return The initial state
     */
    TState GetInitialState() const { return initial_state_; }

    /**
     * @brief Gets a string representation of a state (optional override)
     *
     * Provides a default string representation of states for debugging
     * and display purposes. Derived classes can override this method
     * to provide more meaningful representations.
     *
     * @param state The state to convert to string
     * @return String representation of the state
     */
    virtual std::string GetStateString(const TState& state) const {
        return "State (no custom representation)";
    }

    /**
     * @brief Calculates the heuristic value for a given state (optional)
     *
     * The heuristic function estimates the cost from the given state to
     * the nearest goal state. Used by informed search algorithms like A*
     * and greedy best-first search. Default implementation must return 0
     * (admissible but not informative).
     *
     * @param state The state to evaluate
     * @return Heuristic estimate of cost to goal
     */
    virtual CostType Heuristic(const TState& state) const = 0;
};

#endif  // SEARCH_ALG_DATA_STRUCTURE_PROBLEM_H_