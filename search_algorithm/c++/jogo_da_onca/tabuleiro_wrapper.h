#ifndef TABULEIRO_WRAPPER_H_
#define TABULEIRO_WRAPPER_H_

#include <stdexcept>

#include "data_structure/adversarial_search/games/adugo_game.h"
#include "server/tabuleiro.h"

using namespace adugo_game;

class TimeoutException : public std::runtime_error {
   public:
    TimeoutException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Wrapper class for the tabuleiro c library in server/library.
 * @note This class provides an interface to interact with the tabuleiro
 * functionalities, no need to use the tabuleiro c library directly.
 */
class TabuleiroWrapper {
   public:
    TabuleiroWrapper() = default;
    ~TabuleiroWrapper() = default;

    /**
     * @brief Connects to the redis server.
     * @param argc Argument count from main.
     * @param argv Argument vector from main.
     */
    void ConnectToServer(int argc, char** argv) {
        tabuleiro_conecta(argc, argv);
    }

    /**
     * @brief Sends an action to the server.
     * @param player The player making the action.
     * @param action The action to send.
     */
    void SendAction(const Player& player, const Action& action);

    /**
     * @brief Receives the current state from the server.
     * @param timeout_seconds Maximum seconds to wait for server response (default: 20)
     * @return The current game state.
     * @throws TimeoutException if no response received within timeout
     */
    State ReceiveState(int timeout_seconds = 20);

   private:
    State GetStateFromBoardString(const std::string& server_board,
                                  const Player& my_player);
};
#endif  // TABULEIRO_WRAPPER_H_