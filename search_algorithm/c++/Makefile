# Makefile for C++ Search Algorithm Project
# Author: Generated for search_algorithm project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -g 
INCLUDES = -I. -Idata_structure -Ialgorithms
LDFLAGS = -lncurses
RELAXED_FLAGS = -Wno-unused-parameter -Wno-unused-variable

# Directories
SRC_DIR = .
DATA_STRUCTURE_DIR = data_structure
PROBLEMS_DIR = data_structure/basic_search/problems
BASIC_ALGORITHMS_DIR = algorithms/basic_search
BASIC_ALGORITHMS_VISUAL_DIR = algorithms/basic_search/visual
BUILD_DIR = build
BIN_DIR = bin

MAIN_FILE = chess_main.cc
#MAIN_FILE = tile_main.cc
EXAMPLES_DIR = examples

# Source files
SOURCES = $(wildcard $(PROBLEMS_DIR)/*.cc)

# Object files
OBJECTS = $(SOURCES:%.cc=$(BUILD_DIR)/%.o)

# Header files (for dependencies)
HEADERS = $(wildcard $(DATA_STRUCTURE_DIR)/*.h) \
          $(wildcard $(DATA_STRUCTURE_DIR)/*.tpp) \
          $(wildcard $(PROBLEMS_DIR)/*.h) \
          $(wildcard $(ALGORITHMS_DIR)/*.h) \
          $(wildcard $(ALGORITHMS_DIR)/*.tpp) \
          $(wildcard $(VISUAL_DIR)/*.h) \
          $(wildcard $(VISUAL_DIR)/*.tpp)

# Target executable
TARGET = $(BIN_DIR)/$(basename $(MAIN_FILE))

# Examples
EXAMPLE_SOURCES = $(wildcard $(EXAMPLES_DIR)/*.cc)
EXAMPLE_TARGETS = $(EXAMPLE_SOURCES:$(EXAMPLES_DIR)/%.cc=$(BIN_DIR)/%)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)/$(PROBLEMS_DIR)
	@mkdir -p $(BIN_DIR)

# Main target
$(TARGET): $(OBJECTS) $(MAIN_FILE)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(MAIN_FILE) $(OBJECTS) $(LDFLAGS)

# Object files rule
$(BUILD_DIR)/%.o: %.cc $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Examples target - compile all examples
examples: directories $(EXAMPLE_TARGETS)

# Rule to compile each example
$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.cc $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(OBJECTS) $(LDFLAGS)

# Test target (if you want to create a test executable)
test: directories $(OBJECTS) test_main.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_DIR)/test_search test_main.cc $(OBJECTS) $(LDFLAGS)

# Debug build
debug: CXXFLAGS += -DDEBUG -g3 -O0
debug: all

# Relaxed build (less warnings)
relaxed: CXXFLAGS += $(RELAXED_FLAGS)
relaxed: all

# Release build
release: CXXFLAGS += -DNDEBUG -O3
release: all

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Clean and rebuild
rebuild: clean all

# Show compiler and flag information
info:
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Includes: $(INCLUDES)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Headers: $(HEADERS)"
	@echo "Example Sources: $(EXAMPLE_SOURCES)"
	@echo "Example Targets: $(EXAMPLE_TARGETS)"

# Install (copy to system path - optional)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/search_algorithm

# Format code using clang-format (if available)
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		find . -name "*.h" -o -name "*.cc" -o -name "*.cpp" -o -name "*.tpp" | xargs clang-format -i; \
		echo "Code formatted with clang-format"; \
	else \
		echo "clang-format not found. Please install it for code formatting."; \
	fi

# Run static analysis with cppcheck (if available)
check:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c++17 --inline-suppr $(SRC_DIR); \
	else \
		echo "cppcheck not found. Please install it for static analysis."; \
	fi

# Generate documentation with Doxygen (if available)
docs:
	@if command -v doxygen >/dev/null 2>&1; then \
		doxygen Doxyfile; \
	else \
		echo "doxygen not found. Please install it for documentation generation."; \
	fi

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  examples  - Build all example executables"
	@echo "  test      - Build test executable"
	@echo "  debug     - Build with debug flags"
	@echo "  release   - Build with release optimizations"
	@echo "  relaxed   - Build with relaxed warnings"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and build"
	@echo "  info      - Show build information"
	@echo "  format    - Format code with clang-format"
	@echo "  check     - Run static analysis with cppcheck"
	@echo "  docs      - Generate documentation with Doxygen"
	@echo "  install   - Install to system path"
	@echo "  uninstall - Remove from system path"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all directories examples test debug release clean rebuild info install uninstall format check docs help relaxed

# Dependency tracking (automatically generated)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: %.cc
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT '$(@:.d=.o)' $< > $@
