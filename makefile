CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
CXXFLAGS_DEBUG = -std=c++17 -Wall -Wextra -O0 -g -DDEBUG
CFLAGS = -Wall -Wextra -O2
CFLAGS_DEBUG = -Wall -Wextra -O0 -g -DDEBUG
INCLUDES = -I. -I./data_structure/adversarial_search -I./algorithms/adversarial_search
LDFLAGS = -lhiredis

# Directories
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin
OBJ_DIR = $(BUILD_DIR)/obj
OBJ_DIR_DEBUG = $(BUILD_DIR)/obj_debug

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

# Debug object files
ADUGO_GAME_OBJ_DEBUG = $(OBJ_DIR_DEBUG)/adugo_game.o
TABULEIRO_OBJ_DEBUG = $(OBJ_DIR_DEBUG)/tabuleiro_wrapper.o
SERVER_OBJ_DEBUG = $(OBJ_DIR_DEBUG)/tabuleiro.o
MAIN_OBJ_DEBUG = $(OBJ_DIR_DEBUG)/adugo_main.o
MANUAL_PLAY_OBJ_DEBUG = $(OBJ_DIR_DEBUG)/adugo_manual_play.o

# Targets
ADUGO_MAIN = $(BIN_DIR)/adugo_main
ADUGO_MANUAL = $(BIN_DIR)/adugo_manual_play
ADUGO_MAIN_DEBUG = $(BIN_DIR)/adugo_main_debug
ADUGO_MANUAL_DEBUG = $(BIN_DIR)/adugo_manual_play_debug

.PHONY: all clean directories debug

all: directories $(ADUGO_MAIN) $(ADUGO_MANUAL)

debug: directories $(ADUGO_MAIN_DEBUG) $(ADUGO_MANUAL_DEBUG)

directories:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR) $(OBJ_DIR_DEBUG)

# Build adugo_main (release)
$(ADUGO_MAIN): $(MAIN_OBJ) $(ADUGO_GAME_OBJ) $(TABULEIRO_OBJ) $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Build adugo_manual_play (release)
$(ADUGO_MANUAL): $(MANUAL_PLAY_OBJ) $(ADUGO_GAME_OBJ) $(TABULEIRO_OBJ) $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Build adugo_main (debug)
$(ADUGO_MAIN_DEBUG): $(MAIN_OBJ_DEBUG) $(ADUGO_GAME_OBJ_DEBUG) $(TABULEIRO_OBJ_DEBUG) $(SERVER_OBJ_DEBUG)
	$(CXX) $(CXXFLAGS_DEBUG) $^ -o $@ $(LDFLAGS)

# Build adugo_manual_play (debug)
$(ADUGO_MANUAL_DEBUG): $(MANUAL_PLAY_OBJ_DEBUG) $(ADUGO_GAME_OBJ_DEBUG) $(TABULEIRO_OBJ_DEBUG) $(SERVER_OBJ_DEBUG)
	$(CXX) $(CXXFLAGS_DEBUG) $^ -o $@ $(LDFLAGS)

# Compile C++ sources (release)
$(OBJ_DIR)/adugo_game.o: data_structure/adversarial_search/games/adugo_game.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/tabuleiro_wrapper.o: jogo_da_onca/tabuleiro_wrapper.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/adugo_main.o: jogo_da_onca/adugo_main.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/adugo_manual_play.o: tests/adugo_manual_play.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source (release)
$(OBJ_DIR)/tabuleiro.o: jogo_da_onca/server/tabuleiro.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile C++ sources (debug)
$(OBJ_DIR_DEBUG)/adugo_game.o: data_structure/adversarial_search/games/adugo_game.cc
	$(CXX) $(CXXFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

$(OBJ_DIR_DEBUG)/tabuleiro_wrapper.o: jogo_da_onca/tabuleiro_wrapper.cc
	$(CXX) $(CXXFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

$(OBJ_DIR_DEBUG)/adugo_main.o: jogo_da_onca/adugo_main.cc
	$(CXX) $(CXXFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

$(OBJ_DIR_DEBUG)/adugo_manual_play.o: tests/adugo_manual_play.cc
	$(CXX) $(CXXFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

# Compile C source (debug)
$(OBJ_DIR_DEBUG)/tabuleiro.o: jogo_da_onca/server/tabuleiro.c
	$(CC) $(CFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run_main: $(ADUGO_MAIN)
	$(ADUGO_MAIN) o 127.0.0.1 10001

run_manual: $(ADUGO_MANUAL)
	$(ADUGO_MANUAL) o 127.0.0.1 10001

run_main_debug: $(ADUGO_MAIN_DEBUG)
	$(ADUGO_MAIN_DEBUG) o 127.0.0.1 10001

run_manual_debug: $(ADUGO_MANUAL_DEBUG)
	$(ADUGO_MANUAL_DEBUG) o 127.0.0.1 10001