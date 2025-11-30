CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I. -I./data_structure/adversarial_search -I./algorithms/adversarial_search
LDFLAGS = -lhiredis

# Directories
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
ADUGO_GAME_SRC = data_structure/adversarial_search/games/adugo_game.cc
TABULEIRO_SRC = jogo_da_onca/tabuleiro_wrapper.cc
SERVER_SRC = jogo_da_onca/server/tabuleiro.c
MAIN_SRC = jogo_da_onca/adugo_main.cc
MANUAL_PLAY_SRC = tests/adugo_manual_play.cc

# Object files
ADUGO_GAME_OBJ = $(OBJ_DIR)/adugo_game.o
TABULEIRO_OBJ = $(OBJ_DIR)/tabuleiro_wrapper.o
SERVER_OBJ = $(OBJ_DIR)/tabuleiro.o
MAIN_OBJ = $(OBJ_DIR)/adugo_main.o
MANUAL_PLAY_OBJ = $(OBJ_DIR)/adugo_manual_play.o

# Targets
ADUGO_MAIN = $(BIN_DIR)/adugo_main
ADUGO_MANUAL = $(BIN_DIR)/adugo_manual_play

.PHONY: all clean directories

all: directories $(ADUGO_MAIN) $(ADUGO_MANUAL)

directories:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

# Build adugo_main
$(ADUGO_MAIN): $(MAIN_OBJ) $(ADUGO_GAME_OBJ) $(TABULEIRO_OBJ) $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Build adugo_manual_play
$(ADUGO_MANUAL): $(MANUAL_PLAY_OBJ) $(ADUGO_GAME_OBJ) $(TABULEIRO_OBJ) $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile C++ sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/adugo_game.o: data_structure/adversarial_search/games/adugo_game.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/tabuleiro_wrapper.o: jogo_da_onca/tabuleiro_wrapper.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/adugo_main.o: jogo_da_onca/adugo_main.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@	
	
$(OBJ_DIR)/adugo_manual_play.o: tests/adugo_manual_play.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source
$(OBJ_DIR)/tabuleiro.o: jogo_da_onca/server/tabuleiro.c
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run_main: $(ADUGO_MAIN)
	$(ADUGO_MAIN) o 127.0.0.1 10001

run_manual: $(ADUGO_MANUAL)
	$(ADUGO_MANUAL) o 127.0.0.1 10001