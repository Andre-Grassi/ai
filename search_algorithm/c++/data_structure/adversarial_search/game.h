/**
 * @file game.h
 * @brief Base class for adversarial search games
 * @author Andre Grassi
 * @date 2025
 */

#ifndef SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_GAME_H_
#define SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_GAME_H_

#include <iostream>
#include <limits>
#include <memory>
#include <vector>

// Reference: Artificial Intelligence: A Modern Approach 4ed., p. 193

// Boilerplate to create has_is_max test
template <typename T, typename = void>
struct has_is_max : std::false_type {};

template <typename T>
struct has_is_max<T, std::void_t<decltype(std::declval<T>().IsMax())>>
    : std::true_type {};

// Auxiliar variable for easier usage
template <typename T>
inline constexpr bool has_is_max_v = has_is_max<T>::value;

/**
 * @brief Class for games to be used in adversarial search
 * @tparam TState: the type of the state representation.
 * @tparam TAction: the type of the action representation.
 * @tparam TUtility: the type of the utility value. If it is not a primitive
 * type, then you must implement operators for comparison.
 * @tparam TPlayer: the struct or class representing the player, which MUST
 * have a IsMax public method. That will be used in minimax search.
 */
template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
class Game {
   public:
    /**
     * @brief Verifies in compilaion time if the TPlayer has IsMax method
     */
    static_assert(
        has_is_max_v<TPlayer>,
        "The type TPlayer used in Game<> must have a method 'bool IsMax()'.");

    Game(const TState& initial_state,
         int max_depth = std::numeric_limits<int>::max())
        : initial_state_(initial_state), max_depth_(max_depth) {}

    ~Game() = default;

    /**
     * @brief Gets the player whose turn it is to move in the given state.
     * @param state State to determine which player's turn it is.
     * @return Player that can move.
     */
    virtual TPlayer GetPlayerToMove(const TState& state) const = 0;

    /**
     * @brief Gets all legal moves for the player to move in the given state.
     * @param state State to get legal moves from.
     * @return The vector of legal moves in the state given
     */
    virtual std::vector<TAction> GetActions(const TState& state) const = 0;

    /**
     * @brief Gets the pointer to state that results from taking an action in a
     * state.
     * @param state State to take action from.
     * @param action Action to apply.
     * @return Pointer to the state resulting from taking the action, or nullptr
     * if action is invalid
     */
    virtual std::unique_ptr<TState> GetResult(const TState& state,
                                              const TAction& action) const = 0;

    /**
     * @brief Determines if the given state is a terminal state (game over).
     * @param state State to check for terminal condition.
     * @return True when the game is over, and false otherwise
     * @note States where the game has ended are called terminal states
     */
    virtual bool IsTerminal(const TState& state) const = 0;

    /**
     * @brief (Optional) Determines if the given state is a cutoff state.
     * @param state State to check for cutoff condition.
     * @param depth Current depth in the search tree.
     * @return True when the search should be cut off at this state (including
     * terminal states), false otherwise.
     * @note This method is optional and can be overridden in derived classes in
     * case of using depth-limited search algorithms, such as heuristic minimax.
     * @note Not only the depth can be used to determine cutoff, but also
     * features of the state itself.
     */
    virtual bool IsCutoff(const TState& /*state*/, int /*depth*/) const {
        return false;
    }

    /**
     * @brief Gets the utility value of a terminal state.
     * @param state Terminal state to evaluate.
     * @return The payoff for the player when the games ends in the given state.
     * @note The utility is from the point of view of the MAX player always.
     * @warning This method should only be called on terminal states, otherwise
     * will throw an error.
     */
    virtual TUtility GetUtility(const TState& state) const = 0;

    /**
     * @brief (Optional) Gets the heuristic evaluation of a state.
     * @param state State to evaluate.
     * @return The estimate of the expected utility of the given state.
     * @note This method is optional and can be overridden in derived classes in
     * case of using heuristic search algorithms, such as heuristic minimax.
     * @note The evaluation is from the point of view of the MAX player always.
     * @note Unlike GetUtility, this method can be called on any state.
     * @note For terminal states, GetEval = GetUtility, for non-terminal states,
     * Utility(loss) <= GetEval <= Utility(win).
     */
    virtual TUtility GetEval(const TState& /*state*/) const {
        throw std::logic_error("GetEval not implemented for this game");
    }

    virtual TState GetInitialState() const { return initial_state_; }

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
    virtual std::string GetStateString(const TState& /*state*/) const {
        return "State (no custom representation)";
    }

    virtual void PrintState(const TState& state) const {
        std::cout << GetStateString(state);
    }

   protected:
    TState initial_state_;
    int max_depth_;  ///< Maximum search depth for depth-limited algorithms,
                     ///< default is maximum int value, which is impossible to
                     ///< compute for any reasonable game tree. So change it
                     ///< when needed.
};

#endif  // SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_GAME_H_