#include "adugo_game.h"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace adugo_game;

Player AdugoGame::GetPlayerToMove(const State& state) const {
    // If the game has ended, no player can play
    if (IsTerminal(state)) return Player(Symbol::kEmpty);

    return Player(this->playerToMove);
}

//--------------------IMPLEMENTOBA
//state eh um vetor de 35 Symbol, tem q ver na enum no .h quais sao cada pra
//tratar as movimentacoes de cada bixo do jeito certo
std::vector<Action> AdugoGame::GetPlayerActions(const State& state, Player player) const {
    std::vector<Action> actions;
    //mapear jogadas possiveis de onca/cachorros <<-- depende das infos de 'player'
    //usar de exemplo o codigo do jogo da velha
    return actions;
}

//--------------------IMPLEMENTOBA
std::unique_ptr<State> AdugoGame::GetResult(const State& state, const Action& action) const {
    return nullptr;
}

std::vector<Action> AdugoGame::GetActions(const State& state) const {
    return GetPlayerActions(state, GetPlayerToMove(state));
}

bool AdugoGame::IsTerminal(const State& state) const {
    if (CalculateWinner(state) == Symbol::kEmpty)//se ninguem ganhou, falso
        return false;
    return true;//alguem ganhou
}

//-------- mexi quase nd, n sei se ta certo ja ou nem
Utility AdugoGame::GetUtility(const State& state) const {
    if (!IsTerminal(state))
        throw std::logic_error("GetUtility called on non-terminal state");

    Player winner = CalculateWinner(state);

    if (winner == Player(Symbol::kEmpty)) return 0;  // Draw
    if (winner == Player(Symbol::kC)) return 1;      // C Win
    return -1;                                       // O win
}

Player AdugoGame::CalculateWinner(const State& state) const {
    Symbol reference_symbol;

    //onca ganha se matar 5 cachorros
    int dog_count = 0;
    int dog_limit = 9;//14-5 | total dogos - 5 mortos, enum?
    for (Symbol v : state)
        if (v == Symbol::kC)
            dog_count++;
    if (dog_count <= dog_limit)
        return Player(Symbol::kO);
 
    // verifica se cachorros ganharam
    for (int i = 0; i < kGridDimension; i++)
        if (state[i] == Symbol::kO){
            //coloca onca como jogadora pra verificar as acoes possiveis
            if (GetPlayerActions(state, Player(Symbol::kO)).size()) //verifica se onca possui movimentos validos
                reference_symbol = Symbol::kEmpty; //se tiver, ninguem venceu ainda
            else
                reference_symbol = Symbol::kC; //cachorros imobilizaram a onca

            break;//soh existe uma onca, pode sair do loop
        }
    return Player(reference_symbol);
}

std::string AdugoGame::GetStateString(const State& state) const {
    std::vector<std::string> lines = {
        "  1    2    3    4    5",
        "1 %0----%1----%2----%3----%4",
        "  |\\   |   /|\\   |   /|",
        "  | \\  |  / | \\  |  / |",
        "  |  \\ | /  |  \\ | /  |",
        "  |   \\|/   |   \\|/   |",
        "2 %5----%6----%7----%8----%9",
        "  |   /|\\   |   /|\\   |",
        "  |  / | \\  |  / | \\  |",
        "  | /  |  \\ | /  |  \\ |",
        "  |/   |   \\|/   |   \\|",
        "3 %10----%11----%12----%13----%14",
        "  |\\   |   /|\\   |   /|",
        "  | \\  |  / | \\  |  / |",
        "  |  \\ | /  |  \\ | /  |",
        "  |   \\|/   |   \\|/   |",
        "4 %15----%16----%17----%18----%19",
        "  |   /|\\   |   /|\\   |",
        "  |  / | \\  |  / | \\  |",
        "  | /  |  \\ | /  |  \\ |",
        "  |/   |   \\|/   |   \\|",
        "5 %20----%21----%22----%23----%24",
        "           /|\\",
        "          / | \\",
        "         /  |  \\",
        "        /   |   \\",
        "6      %26----%27----%28",
        "      /     |     \\",
        "     /      |      \\",
        "    /       |       \\",
        "   /        |        \\",
        "7 %30---------%32---------%34"
    };

    std::stringstream s;
    for (auto &line : lines) {
        for (int i = kGridDimension-1; i >=0; --i) {
            std::string key = "%" + std::to_string(i);
            size_t pos = line.find(key);
            if (pos != std::string::npos){
                std::string stateString = std::string(1, (char)state[i]);
                line.replace(pos, key.size(), stateString);
            }
        }
        s << line << '\n';
    }

    return s.str();
}

