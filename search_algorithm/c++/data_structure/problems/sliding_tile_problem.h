/**
 * @file sliding_tile_problem.h
 * @brief Implementation of a specific problem: the sliding tile puzzle
 * (8-puzzle, 15-puzzle, etc.)
 * @author Andre Grassi
 * @date 2025
 */

#ifndef SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_SLIDING_H_
#define SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_SLIDING_H_

#include <vector>

#include "node.h"
#include "problem.h"

#define BLANK_TILE 0  ///< Value representing the blank tile in the puzzle

/**
 * @namespace sliding_tile
 * @brief Contains classes and types for sliding tile problem implementation
 *
 * This namespace encapsulates all components needed for solving sliding tile
 * problems using various search algorithms. The sliding tile problem (also
 * known as the 8-puzzle for 3x3 grids) consists of numbered tiles in a grid
 * with one blank space, where the goal is to arrange tiles in numerical order.
 */
namespace sliding_tile {

/**
 * @brief Available actions in the sliding tile puzzle
 *
 * Actions represent moving the blank tile in one of four directions.
 * The action names indicate the direction the blank tile moves.
 */
enum class Action {
    kUp,    ///< Move blank tile up
    kDown,  ///< Move blank tile down
    kLeft,  ///< Move blank tile left
    kRight  ///< Move blank tile right
};

/**
 * @brief State representation as a 2D grid of tiles
 *
 * Each state is represented as a vector of vectors containing tile numbers.
 * The value 0 (BLANK_TILE) represents the empty space in the puzzle.
 */
using State = std::vector<std::vector<uint64_t>>;

using CostType = int;  ///< Cost type for actions (uniform cost of 1)

/**
 * @brief Sliding tile puzzle problem implementation
 *
 * This class implements the sliding tile puzzle as a search problem.
 * It supports variable dimensions (3x3, 4x4, etc.) and provides both
 * random initial state generation and custom initial state specification.
 *
 * The puzzle consists of numbered tiles arranged in a grid with one blank
 * space. The goal is to arrange tiles in ascending order with the blank
 * in the bottom-right corner.
 *
 * @note The class ensures generated puzzles are solvable by checking
 *       the inversion count and blank tile position parity.
 */
class SlidingTileProblem : public Problem<State, Action, CostType> {
   private:
    uint64_t dimension_ = 3;  ///< Grid dimension (3 for 3x3, 4 for 4x4, etc.)
    State goal_state_;        ///< Target configuration to reach

    /**
     * @brief Generates a random solvable puzzle configuration
     *
     * Creates a randomized initial state that is guaranteed to be solvable.
     * Uses the inversion count rule to ensure solvability.
     *
     * @return Random solvable state
     * @note Only generates valid, solvable configurations
     */
    State RandomizeBoard();

    /**
     * @brief Checks if a given state is solvable
     *
     * Determines puzzle solvability using the inversion count rule:
     * - For odd grid width: puzzle is solvable if inversion count is even
     * - For even grid width (NOT IMPLEMENTED): depends on blank position and
     * inversion count
     *
     * @param state The state to check for solvability
     * @return true if the state is solvable, false otherwise
     */
    bool IsSolvable(const State& state) const;

    /**
     * @brief Generates the goal state for the current dimension
     *
     * Creates the target configuration with tiles arranged in ascending
     * order from 1 to (dimension²-1), with the blank tile at first position.
     *
     * @return Goal state for the current puzzle dimension
     */
    State GenerateGoalState() const;

   public:
    /**
     * @brief Constructs puzzle with specified initial state and dimension
     *
     * @param initial_state The starting configuration
     * @param dimension Grid size (3 for 3x3, 4 for 4x4, etc.)
     * @warning Does not verify if the initial state is solvable
     */
    SlidingTileProblem(const State& initial_state, const uint64_t dimension)
        : Problem<State, Action, CostType>(initial_state),
          dimension_(dimension),
          goal_state_(GenerateGoalState()) {}

    /**
     * @brief Constructs puzzle with random solvable initial state
     *
     * @param dimension Grid size (3 for 3x3, 4 for 4x4, etc.)
     * @note Automatically generates a solvable random initial configuration
     * @note We need to call the base class constructor first, that's why
     * the initial state is passed as a dummy state and then the dimension is
     * set and the initial state is overwritten with a random board.
     */
    SlidingTileProblem(const uint64_t dimension)
        : Problem<State, Action, CostType>(
              State(dimension, std::vector<uint64_t>(dimension, 0))),
          dimension_(dimension),
          goal_state_(GenerateGoalState()) {
        this->initial_state_ = RandomizeBoard();
    }

    /**
     * @brief Virtual destructor
     */
    virtual ~SlidingTileProblem() = default;

    /**
     * @brief Tests if a state is the goal state
     *
     * @param state The state to test
     * @return true if the state matches the goal configuration
     */
    virtual bool IsGoal(const State& state) const override {
        return state == goal_state_;
    }

    /**
     * @brief Gets all valid actions from a given state
     *
     * Returns actions that can be applied without moving the blank tile
     * outside the grid boundaries.
     *
     * @param state The current state
     * @return Vector of valid actions (kUp, kDown, kLeft, kRight)
     */
    virtual std::vector<Action> GetActions(const State& state) const override;

    /**
     * @brief Applies an action to a state and returns the resulting state
     *
     * Moves the blank tile in the specified direction by swapping it with
     * the adjacent tile. Returns nullptr if the action is invalid.
     *
     * @param state The current state
     * @param action The action to apply
     * @return Unique pointer to new state, or nullptr if action is invalid
     */
    virtual std::unique_ptr<State> GetResult(
        const State& state, const Action& action) const override;

    /**
     * @brief Gets the cost of applying an action
     *
     * All actions have uniform cost of 1.0 in the sliding tile puzzle.
     *
     * @param state Current state (unused)
     * @param action Action being applied (unused)
     * @param new_state Resulting state (unused)
     * @return Always returns 1.0 (uniform cost)
     */
    virtual CostType GetActionCost(const State&, const Action&,
                                   const State&) const override {
        return 1;  // Uniform cost for all actions
    }

    /**
     * @brief Calculates heuristic value for a state
     *
     * Computes the Manhattan distance heuristic, which is the sum of
     * distances each tile must move to reach its goal position.
     * This heuristic is admissible for A* search.
     *
     * @param state The state to evaluate
     * @return Manhattan distance to goal (admissible heuristic)
     */
    CostType Heuristic(const State& state) const override;

    /**
     * @brief Gets the grid dimension
     * @return The dimension of the grid (3 for 3x3, 4 for 4x4, etc.)
     */
    uint64_t GetDimension() const { return dimension_; }

    /**
     * @brief Finds the position of the blank tile
     *
     * Searches the state for the blank tile (value 0) and returns its
     * coordinates as a pair.
     *
     * @param state The state to search
     * @return Pair (row, col) of blank tile position, (-1, -1) if not found
     */
    std::pair<int, int> GetBlankTileIndex(const State& state) const;

    /**
     * @brief Gets the goal state
     * @return The target configuration for this puzzle
     */
    State GetGoalState() const { return goal_state_; }

    /**
     * @brief Prints a state to console in a readable format
     * @param state The state to print
     */
    void PrintState(const State& state) const;

    std::string GetStateString(const State& state) const override;
};

};  // namespace sliding_tile

#endif  // SEARCH_ALG_DATA_STRUCTURE_PROBLEMS_SLIDING_H_