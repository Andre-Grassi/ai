#include "adugo_game.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace adugo_game;

const std::pair<std::vector<int>, std::vector<int>>* VerifyInMap(
    int position) {  // verifica se o ponto pertence ao tabuleiro
    auto homeowner = kGridDimensionNeighborhood.find(
        position);  // retorna um ponteiro para o pair do map se existir no
                    // tabuleiro
    if (homeowner == kGridDimensionNeighborhood.end()) return nullptr;
    return &(homeowner->second);
}

Player AdugoGame::GetPlayerToMove(const State& state) const {
    // If the game has ended, no player can play
    if (IsTerminal(state)) return Player(Symbol::kEmpty);

    return state.player_to_move;
}

bool AdugoGame::IsNeighbor(int position1, int position2) const {
    const std::pair<std::vector<int>, std::vector<int>>* neighbor =
        VerifyInMap(position1);
    if (!neighbor)
        return false;  // se ta fora do mapa, nao e vizinho de
                       // ninguem

    if (std::find(neighbor->first.begin(), neighbor->first.end(), position2) !=
        neighbor->first.end())
        return true;  // se achou no mapa de vizinhanca, e vizinho

    return false;
}

std::optional<int> AdugoGame::FindMiddlePosition(int position1, int position3)
    const {  // retorna a posicao no meio de outras 2, se estiverem as 3
             // alinhadas
    std::vector<int> possibilities;

    auto collection1 = VerifyInMap(position1);
    auto collection3 = VerifyInMap(position3);

    if (!collection1 || !collection3) return std::nullopt;

    if (IsNeighbor(position1,
                   position3))  // se sao vizinhos diretos, nao ha ponto no meio
        return std::nullopt;

    for (const auto neighbor : collection1->first) {
        if (std::find(collection3->first.begin(), collection3->first.end(),
                      neighbor) != collection3->first.end())
            possibilities.push_back(neighbor);
    }

    if (possibilities.size() == 1) {
        return possibilities[0];
    }

    return std::nullopt;
}

std::vector<int> AdugoGame::FindCommonConnections(int position1,
                                                  int position2) const {
    std::vector<int> common_lines;

    const std::pair<std::vector<int>, std::vector<int>>* tiger =
        VerifyInMap(position1);
    const std::pair<std::vector<int>, std::vector<int>>* dog =
        VerifyInMap(position2);

    if (!tiger || !dog) return common_lines;

    for (int tiger_pos :
         tiger->second) {  // separa as retas comuns entre os pontos
        for (int dog_pos : dog->second) {
            if (tiger_pos == dog_pos) common_lines.emplace_back(tiger_pos);
        }
    }

    return common_lines;
}

bool AdugoGame::IsAligned(int position1, int position2, int position3,
                          std::vector<int> common_lines) const {
    if (position1 == position2 || position1 == position3 ||
        position2 == position3)
        return false;  // necessario 3 pontos distintos para o abate

    if (IsNeighbor(position1, position2) &&
        IsNeighbor(position2, position3)) {  // precisam ser 3 pontos vizinhos
        const std::pair<std::vector<int>, std::vector<int>>*
            empty_possibilities = VerifyInMap(position3);
        if (!empty_possibilities)  // se nao tem 3 ponto, nao tem porque ver se
                                   // ta alinhado
            return false;
        for (int emp : empty_possibilities->second) {
            for (int com : common_lines) {  // se houver reta comum entre os 3
                                            // pontos, estao alinhados
                if (emp == com) return true;
            }
        }
    }
    return false;
}

void AdugoGame::AddIndirectNeighbors(const State& state, Player player,
                                     std::vector<Action>& actions,
                                     int original_position,
                                     int current_position) const {
    const std::pair<std::vector<int>, std::vector<int>>* neighbors =
        VerifyInMap(current_position);
    if (!neighbors) return;  // se current_position nao ta no tabuleiro, retorna

    for (int neighbor :
         neighbors->first) {  // pra cada vizinho do cachorro, verifica se ha
                              // casa vazia e alinhada com cachorro/onca
        if (state[neighbor] == Symbol::kEmpty &&
            IsAligned(original_position, current_position, neighbor,
                      this->FindCommonConnections(original_position,
                                                  current_position)))
            actions.emplace_back(player.symbol, original_position, neighbor);
    }

    return;
}

std::vector<Action> AdugoGame::GetPlayerActions(const State& state,
                                                Player player) const {
    std::vector<Action> actions;

    for (int i = 0; i < kGridDimension; i++) {
        if (state[i] == player.symbol) {
            auto homeowner = kGridDimensionNeighborhood.find(i);
            if (homeowner == kGridDimensionNeighborhood.end())
                continue;  // posicao do player fora do mapa
                           // continua para caso for cachorro

            const std::pair<std::vector<int>, std::vector<int>>& neighbors =
                homeowner->second;

            for (int neighbor_pos : neighbors.first) {
                if (player.symbol == Symbol::kO) {
                    if (state[neighbor_pos] == Symbol::kEmpty)
                        actions.emplace_back(player.symbol, homeowner->first,
                                             neighbor_pos);
                    else if (state[neighbor_pos] == Symbol::kC)
                        AddIndirectNeighbors(state, player, actions,
                                             homeowner->first, neighbor_pos);
                } else if (player.symbol == Symbol::kC &&
                           state[neighbor_pos] == Symbol::kEmpty) {
                    actions.emplace_back(player.symbol, homeowner->first,
                                         neighbor_pos);
                }
            }
        }
    }

    return actions;
}

std::unique_ptr<State> AdugoGame::GetResult(const State& state,
                                            const Action& action) const {
    if (IsTerminal(state)) return nullptr;

    Player player = GetPlayerToMove(state);
    Symbol action_symbol = action.player_symbol;
    int ply_index = action.cell_index_origin;
    int dest_index = action.cell_index_destination;

    if (action_symbol != player.symbol) return nullptr;

    if (state[dest_index] == Symbol::kBlock) return nullptr;

    std::unique_ptr<State> new_state = std::make_unique<State>(state);

    if (!IsNeighbor(ply_index,
                    dest_index)) {  // caso da onca matar o cachoro, limpa a
                                    // antiga casa do cachorro
        auto middle = FindMiddlePosition(
            ply_index, dest_index);  // se ha somente um vizinho entre ambos e
                                     // os 3 estao alinhados, houve abate
        if (middle.has_value() &&
            IsAligned(ply_index, middle.value(), dest_index,
                      FindCommonConnections(ply_index, dest_index)))
            (*new_state)[middle.value()] = Symbol::kEmpty;
    }

    (*new_state)[ply_index] = Symbol::kEmpty;  // player sai da posicao original
    (*new_state)[dest_index] = action_symbol;  // player chega ao destino

    // Switch turn to the other player
    if (player.symbol == Symbol::kC)
        new_state->player_to_move = Player(Symbol::kO);
    else
        new_state->player_to_move = Player(Symbol::kC);

    return new_state;
}

std::vector<Action> AdugoGame::GetActions(const State& state) const {
    return GetPlayerActions(state, GetPlayerToMove(state));
}

bool AdugoGame::IsTerminal(const State& state) const {
    if (CalculateWinner(state) == Symbol::kEmpty)  // se ninguem ganhou, falso
        return false;
    return true;  // alguem ganhou
}

// eai??
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

    // onca ganha se matar 5 cachorros
    int dog_count = 0;
    int dog_limit = 9;  // 14-5 | total dogos - 5 mortos, enum?
    for (Symbol v : state.board)
        if (v == Symbol::kC) dog_count++;
    if (dog_count <= dog_limit) return Player(Symbol::kO);

    // verifica se cachorros ganharam
    for (int i = 0; i < kGridDimension; i++)
        if (state[i] == Symbol::kO) {
            // coloca onca como jogadora pra verificar as acoes possiveis
            if (GetPlayerActions(state, Player(Symbol::kO))
                    .size())  // verifica se onca possui movimentos validos
                reference_symbol =
                    Symbol::kEmpty;  // se tiver, ninguem venceu ainda
            else
                reference_symbol = Symbol::kC;  // cachorros imobilizaram a onca

            break;  // soh existe uma onca, pode sair do loop
        }
    return Player(reference_symbol);
}

// p imprimir bonitinho
std::string AdugoGame::GetStateString(const State& state) const {
    std::vector<std::string> lines = {"  1    2    3    4    5",
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
                                      "7 %30---------%32---------%34"};

    std::stringstream s;
    for (auto& line : lines) {
        for (int i = kGridDimension - 1; i >= 0; --i) {
            std::string key = "%" + std::to_string(i);
            size_t pos = line.find(key);
            if (pos != std::string::npos) {
                std::string stateString = std::string(1, (char)state[i]);
                line.replace(pos, key.size(), stateString);
            }
        }
        s << line << '\n';
    }

    return s.str();
}
