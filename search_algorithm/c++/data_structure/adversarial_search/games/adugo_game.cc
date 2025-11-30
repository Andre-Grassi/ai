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

const std::vector<int>* GetNeighbors(
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
    const std::vector<int>* neighbors = GetNeighbors(position1);
    if (!neighbors)
        return false;  // se ta fora do mapa, nao e vizinho de
                       // ninguem

    if (std::find(neighbors->begin(), neighbors->end(), position2) !=
        neighbors->end())
        return true;  // se achou no mapa de vizinhanca, é vizinho

    return false;
}

std::optional<int> AdugoGame::FindMiddlePosition(int position1, int position3)
    const {  // retorna a posicao no meio de outras 2, se estiverem as 3
             // alinhadas
    std::vector<int> possibilities;

    auto neighbors1 = GetNeighbors(position1);
    auto neighbors3 = GetNeighbors(position3);

    if (!neighbors1 || !neighbors3) return std::nullopt;

    if (IsNeighbor(position1,
                   position3))  // se sao vizinhos diretos, nao ha ponto no meio
        return std::nullopt;

    // Search for common neighbors between the neighbors of position1 and
    // position3
    for (const auto neighbor : *neighbors1) {
        if (std::find(neighbors3->begin(), neighbors3->end(), neighbor) !=
            neighbors3->end())
            // Check if they are aligned
            if (IsAligned(position1, neighbor, position3))
                possibilities.push_back(neighbor);
    }

    // Couldn't find a middle position, or found more than one (error)
    if (possibilities.size() != 1) return std::nullopt;

    // Return the single middle position found
    return possibilities[0];
}

AdugoGame::Alignment AdugoGame::GetAlignment(int pos1, int pos2) const {
    // Check if they are in valid range
    if (pos1 < 0 || pos1 >= kGridDimension || pos2 < 0 ||
        pos2 >= kGridDimension)
        return Alignment::kNotAligned;

    int row1 = pos1 / kBoardWidth;
    int col1 = pos1 % kBoardWidth;
    int row2 = pos2 / kBoardWidth;
    int col2 = pos2 % kBoardWidth;

    if (row1 == row2) return Alignment::kHorizontal;
    if (col1 == col2) return Alignment::kVertical;
    if (std::abs(row1 - row2) == std::abs(col1 - col2))
        return Alignment::kDiagonal;

    return Alignment::kNotAligned;
}

bool AdugoGame::IsAligned(int starting_jaguar_pos, int middle_dog_pos,
                          int landing_pos) const {
    if (starting_jaguar_pos == middle_dog_pos ||
        starting_jaguar_pos == landing_pos || middle_dog_pos == landing_pos)
        return false;  // necessario 3 pontos distintos para o abate

    // Check if they are in valid rage
    if (starting_jaguar_pos < 0 || starting_jaguar_pos >= kGridDimension ||
        middle_dog_pos < 0 || middle_dog_pos >= kGridDimension ||
        landing_pos < 0 || landing_pos >= kGridDimension)
        return false;

    // Check if they are not direct neighbors
    if (!IsNeighbor(starting_jaguar_pos, middle_dog_pos) ||
        !IsNeighbor(middle_dog_pos, landing_pos))
        return false;

    // Get jaguar to landing pos alignment
    Alignment jaguar_landing_alignment =
        GetAlignment(starting_jaguar_pos, landing_pos);

    // Get jaguar to middle dog alignment
    Alignment jaguar_dog_alignment =
        GetAlignment(starting_jaguar_pos, middle_dog_pos);

    // If the jaguar to middle dog and jaguar to landing pos alignment is the
    // same, they are all aligned and the jaguar can capture
    return jaguar_dog_alignment == jaguar_landing_alignment;
}

void AdugoGame::AddIndirectNeighbors(const State& state, Player player,
                                     std::vector<Action>& actions,
                                     int jaguar_position,
                                     int dog_position) const {
    const std::vector<int>* dog_neighbors = GetNeighbors(dog_position);
    if (!dog_neighbors) return;  // se dog_position nao ta no tabuleiro, retorna

    for (int dog_neighbor : *dog_neighbors) {
        // Check if the neighbor cell is empty and aligned for capture
        if (state[dog_neighbor] == Symbol::kEmpty &&
            IsAligned(jaguar_position, dog_position, dog_neighbor))
            actions.emplace_back(player.symbol, jaguar_position, dog_neighbor);
    }

    return;
}

std::vector<Action> AdugoGame::GetPlayerActions(const State& state,
                                                Player player) const {
    std::vector<Action> actions;

    for (int i = 0; i < kGridDimension; i++) {
        if (state[i] == player.symbol) {
            auto player_position = kGridDimensionNeighborhood.find(i);
            if (player_position == kGridDimensionNeighborhood.end())
                continue;  // posicao do player fora do mapa
                           // continua para caso for cachorro

            const std::vector<int>& neighbors = player_position->second;

            for (int neighbor_pos : neighbors) {
                // Going to an empty neighbor is always a valid move
                if (state[neighbor_pos] == Symbol::kEmpty)
                    actions.emplace_back(player.symbol, player_position->first,
                                         neighbor_pos);

                // For jaguar, also add indirect neighbors: cells that it can
                // go by jumping over a dog (and capturing it), ONLY if there's
                // a dog between the empty cell and the jaguar
                if (player.symbol == Symbol::kO &&
                    state[neighbor_pos] == Symbol::kC)
                    AddIndirectNeighbors(state, player, actions,
                                         player_position->first, neighbor_pos);
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

    // Invalid destination
    if (state[dest_index] == Symbol::kBlock) return nullptr;

    std::unique_ptr<State> new_state = std::make_unique<State>(state);

    // If the move is not to a neighbor, it's a capture move
    if (!IsNeighbor(ply_index, dest_index)) {
        auto captured_dog_pos = FindMiddlePosition(
            ply_index, dest_index);  // se ha somente um vizinho entre ambos e
                                     // os 3 estao alinhados, houve abate

        if (captured_dog_pos == std::nullopt)
            throw std::logic_error(
                "Invalid capture move: no middle position found.");

        if (captured_dog_pos.has_value() &&
            IsAligned(ply_index, captured_dog_pos.value(), dest_index))
            // Clear the captured dog from the board
            (*new_state)[captured_dog_pos.value()] = Symbol::kEmpty;
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
#ifdef IGNORE_TERMINAL_TEST
    return false;
#endif

    if (CalculateWinner(state) == Symbol::kEmpty)  // se ninguem ganhou, falso
        return false;
    return true;  // alguem ganhou
}

// eai??
// FIX Issue 8
Utility AdugoGame::GetUtility(const State& state) const {
    if (!IsTerminal(state))
        throw std::logic_error("GetUtility called on non-terminal state");

    Player winner = CalculateWinner(state);

    if (winner == Player(Symbol::kEmpty)) return 0;  // Draw
    if (winner == Player(Symbol::kC)) return 1;      // C Win
    return -1;                                       // O win
}

// Heuristic


// Heuristic = 1 - 2 * ((captured_dogs * cw + jaguar_mobility * mw) / max_jaguar_score)
Utility AdugoGame::GetEval(const State& state) const {
    // If terminal, can calculate utility directly
    if (IsTerminal(state)) {
        return GetUtility(state);
    }

    Utility dogs_value = 0.0f;
    float capture_weight, mobility_weight;

    if (state.player_to_move == Player(Symbol::kC)) {
        // Weights
        capture_weight = 10.0f;
        mobility_weight = 1.0f;
    } else {
    // Weights
        mobility_weight = 10.0f;
        capture_weight = 1.0f;
    }



    const float max_jaguar_mobility =
        8.0f;  // If the jaguar has all moves available

    const Utility max_jaguar_score =
        (static_cast<float>(kNumDogsToCapture) * capture_weight) +
                                     (max_jaguar_mobility * mobility_weight);

    float captured_dogs = static_cast<float>(
        kNumStartingDogs - CountSymbolsInState(state, Symbol::kC));
    float jaguar_mobility =
        static_cast<float>(GetPlayerActions(state, Player(Symbol::kO)).size());

    Utility raw_jaguar_score =
        (captured_dogs * capture_weight) + (jaguar_mobility * mobility_weight);
    // Normalize to [0, 1]
    Utility normalized_jaguar_score = (raw_jaguar_score / max_jaguar_score);

    // Calculate dogs value based on jaguar score
    // If jaguar score is 0 -> dogs value = 1 (best for dogs)
    // If jaguar score is max -> dogs value = -1 (worst for dogs)
    // You can't simply make it the opposit, because the jaguar score is
    // between 0 and 1.
    dogs_value = 1.0f - (2.0f * normalized_jaguar_score);

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