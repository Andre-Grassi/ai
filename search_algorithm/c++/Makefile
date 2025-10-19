# Makefile for C++ Search Algorithm Project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O3
INCLUDES = -I. -Idata_structure -Ialgorithms
RELAXED_FLAGS = -Wno-unused-parameter -Wno-unused-variable

# Directories
SRC_DIR = .
DATA_STRUCTURE_DIR = data_structure
PROBLEMS_DIR = data_structure/problems
ALGORITHMS_DIR = algorithms
BUILD_DIR = build

MAIN_FILE = chess_main.cc

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

# Target executable
TARGET = $(basename $(MAIN_FILE))

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)/$(PROBLEMS_DIR)

# Main target
$(TARGET): $(OBJECTS) $(MAIN_FILE)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(MAIN_FILE) $(OBJECTS)

# Object files rule
$(BUILD_DIR)/%.o: %.cc $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@


# Debug build
debug: CXXFLAGS += -DDEBUG -g3 -O0
debug: all

# Relaxed build (less warnings)
relaxed: CXXFLAGS += $(RELAXED_FLAGS)
relaxed: all

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

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
.PHONY: all directories debug clean rebuild info format check docs help relaxed

# Dependency tracking (automatically generated)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: %.cc
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT '$(@:.d=.o)' $< > $@
