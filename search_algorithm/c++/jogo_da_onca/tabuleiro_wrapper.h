#ifndef TABULEIRO_WRAPPER_H_
#define TABULEIRO_WRAPPER_H_

#include "data_structure/adversarial_search/games/adugo_game.h"
#include "server/tabuleiro.h"

using namespace adugo_game;

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

    void SendAction(const Player& player, const Action& action);

    /**
     * @brief Receives an action from the server.
     * @return The action received from the server. If no action was made by
     * the opponent, returns default Action().
     */
    State ReceiveState(const Player& my_player);

    bool IsPlayerTurn(const std::string& server_response, const Player& player);

   private:
    State GetStateFromBoardString(const std::string& server_board,
                                  const Player& my_player);
};
#endif  // TABULEIRO_WRAPPER_H_