# Makefile for C++ Search Algorithm Project
# Author: Generated for search_algorithm project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -g
INCLUDES = -I. -Idata_structure -Ialgorithms

# Directories
SRC_DIR = .
DATA_STRUCTURE_DIR = data_structure
PROBLEMS_DIR = data_structure/problems
ALGORITHMS_DIR = algorithms
VISUAL_DIR = algorithms/visual
BUILD_DIR = build
BIN_DIR = bin

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
TARGET = $(BIN_DIR)/search_algorithm

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)/$(PROBLEMS_DIR)
	@mkdir -p $(BIN_DIR)

# Main target
$(TARGET): $(OBJECTS) main.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ main.cc $(OBJECTS)

# Object files rule
$(BUILD_DIR)/%.o: %.cc $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Test target (if you want to create a test executable)
test: directories $(OBJECTS) test_main.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_DIR)/test_search test_main.cc $(OBJECTS)

# Debug build
debug: CXXFLAGS += -DDEBUG -g3 -O0
debug: all

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
	@echo "  test      - Build test executable"
	@echo "  debug     - Build with debug flags"
	@echo "  release   - Build with release optimizations"
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
.PHONY: all directories test debug release clean rebuild info install uninstall format check docs help

# Dependency tracking (automatically generated)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: %.cc
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT '$(@:.d=.o)' $< > $@