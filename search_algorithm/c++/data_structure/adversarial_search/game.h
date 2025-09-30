#ifndef SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_GAME_H_
#define SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_GAME_H_

#include <memory>
#include <vector>

// Reference: Artificial Intelligence: A Modern Approach 4ed., p. 193

/**
 * @brief Class for games to be used in adversarial search
 */
template <typename TState, typename TAction, typename TUtility,
          typename TPlayer>
class Game {
   public:
    Game(const TState& initial_state) : initial_state_(initial_state) {}

    /**
     * @return Index of the player whose turn it is to move in the state given
     */
    virtual int GetPlayerToMove(const TState& state) const = 0;

    /**
     * @return The vector of legal moves in the state given
     */
    virtual std::vector<TAction> GetActions(const TState& state) const = 0;

    /**
     * @return Pointer to the state resulting from taking the action, or nullptr
     * if action is invalid
     */
    virtual std::unique_ptr<TState> GetResult(const TState& state,
                                              const TAction& action) const = 0;

    /**
     * @return True when the game is over, and false otherwise
     * @note States where the game has ended are called terminal states
     */
    virtual bool IsTerminal(const TState& state) const = 0;

    /**
     * @return The payoff for the player when the games ends in the given state
     */
    virtual TUtility GetUtility(const TState& state, const TPlayer& player) const = 0;

   protected:
    TState initial_state_;
};

#endif  // SEARCH_ALG_DATA_STRUCTURE_ADVERSARIAL_GAME_H_