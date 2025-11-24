#include "adugo_game.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace adugo_game;

// Auxiliary function to count symbols in the board.
// For example, to count how many 'C' are in the current state.
int CountSymbolsInState(const State& state, Symbol symbol) {
    int count = 0;
    for (const Symbol cell : state.board)
        if (cell == symbol) count++;

    return count;
}

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
        const std::pair<std::vector<int>, std::vector<int>>* middle_position =
            VerifyInMap(position2);
        if (!middle_position)  // se nao tem o ponto do meio, nao tem
                               // alinhamento
            return false;
        // Check if the middle position shares any of the common lines
        // between position1 and position3
        for (int middle_line : middle_position->second) {
            for (int com : common_lines) {  // se houver reta comum entre os 3
                                            // pontos, estao alinhados
                if (middle_line == com) return true;
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

    // Destination invalid
    if (state[dest_index] == Symbol::kBlock) return nullptr;

    std::unique_ptr<State> new_state = std::make_unique<State>(state);

    if (!IsNeighbor(ply_index,
                    dest_index)) {  // caso da onca matar o cachoro, limpa a
                                    // antiga casa do cachorro
        // Find all common neighbors between origin and destination
        auto origin_neighbors = VerifyInMap(ply_index);
        auto dest_neighbors = VerifyInMap(dest_index);

        if (origin_neighbors && dest_neighbors) {
            std::vector<int> common_neighbors;

            // Collect all common neighbors that have dogs
            for (int origin_n : origin_neighbors->first) {
                for (int dest_n : dest_neighbors->first) {
                    if (origin_n == dest_n && state[origin_n] == Symbol::kC) {
                        common_neighbors.push_back(origin_n);
                    }
                }
            }

            // Find which common neighbor is aligned on the same line
            std::vector<int> common_lines =
                FindCommonConnections(ply_index, dest_index);

            std::vector<int> aligned_candidates;
            for (int candidate : common_neighbors) {
                // Check if this candidate is aligned with origin and
                // destination
                if (IsAligned(ply_index, candidate, dest_index, common_lines)) {
                    aligned_candidates.push_back(candidate);
                }
            }

            // If multiple candidates are aligned, choose the one "closest" to
            // being numerically between origin and destination
            if (!aligned_candidates.empty()) {
                int best_candidate = aligned_candidates[0];
                int min_distance =
                    std::abs(best_candidate - (ply_index + dest_index) / 2);

                for (int candidate : aligned_candidates) {
                    int distance =
                        std::abs(candidate - (ply_index + dest_index) / 2);
                    if (distance < min_distance) {
                        min_distance = distance;
                        best_candidate = candidate;
                    }
                }

                (*new_state)[best_candidate] = Symbol::kEmpty;
            }
        }
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

Utility AdugoGame::GetEval(const State& state) const {
    // If terminal, can calculate utility directly
    if (IsTerminal(state)) {
        return GetUtility(state);
    }

    // Weights
    const int capture_weight = 100;
    const int mobility_weight = 1;

    const int max_jaguar_mobility = 8;  // If the jaguar has all moves available

    const double max_jaguar_score = (kNumDogsToCapture * capture_weight) +
                                    (max_jaguar_mobility * mobility_weight);

    int captured_dogs =
        kNumStartingDogs - CountSymbolsInState(state, Symbol::kC);
    int jaguar_mobility = GetPlayerActions(state, Player(Symbol::kO)).size();

    double raw_jaguar_score =
        (captured_dogs * capture_weight) + (jaguar_mobility * mobility_weight);
    // Normalize to [0, 1]
    double normalized_jaguar_score = (raw_jaguar_score / max_jaguar_score);

    // Calculate dogs value based on jaguar score
    // If jaguar score is 0 -> dogs value = 1 (best for dogs)
    // If jaguar score is max -> dogs value = -1 (worst for dogs)
    double dogs_value = 1.0 - (2.0 * normalized_jaguar_score);

    return dogs_value;
}

Player AdugoGame::CalculateWinner(const State& state) const {
    Symbol reference_symbol;

    // onca ganha se matar 5 cachorros
    int dog_count = CountSymbolsInState(state, Symbol::kC);
    int dog_limit = kNumStartingDogs -
                    kNumDogsToCapture;  // 14-5 | total dogos - 5 mortos, enum?
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

// Print board similar to controlador.c format
std::string AdugoGame::GetStateString(const State& state) const {
    std::stringstream s;

    // Print board with # borders like controlador
    s << "#######\n";

    // Rows 1-5 (indices 0-24)
    for (int row = 0; row < 5; row++) {
        s << "#";
        for (int col = 0; col < 5; col++) {
            int index = row * 5 + col;
            s << static_cast<char>(state[index]);
        }
        s << "#\n";
    }

    // Row 6 (indices 26, 27, 28)
    s << "# ";
    s << static_cast<char>(state[26]);
    s << static_cast<char>(state[27]);
    s << static_cast<char>(state[28]);
    s << " #\n";

    // Row 7 (indices 30, 32, 34)
    s << "#";
    s << static_cast<char>(state[30]);
    s << " ";
    s << static_cast<char>(state[32]);
    s << " ";
    s << static_cast<char>(state[34]);
    s << "#\n";

    s << "#######\n";

    return s.str();
}

int AdugoGame::GetJaguarPosition(const State& state) const {
    for (int i = 0; i < kGridDimension; ++i)
        if (state[i] == Symbol::kO) return i;

    return -1;  // Jaguar not found on the board
}