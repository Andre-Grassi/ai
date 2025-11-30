# Building Instructions

This document explains how to build the C++ search algorithm project.

## Prerequisites

Before building, ensure you have the following installed:

- **CMake** (version 3.28 or higher)
- **C++ compiler** with C++17 support (e.g., g++, clang++)
- **ncurses library** - Required for terminal UI features

### Installing Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get install cmake g++ libncurses5-dev libncurses-dev
```

**Arch Linux:**
```bash
sudo pacman -S cmake gcc ncurses
```

**macOS:**
```bash
brew install cmake ncurses
```

## Building the Project

### Standard Build

1. **Create a build directory**:
   ```bash
   mkdir build
   cd build
   ```

2. **Configure the project with CMake:**
   ```bash
   cmake ..
   ```

3. **Build the project:**
   ```bash
   cmake --build .
   ```
   
   Or use make directly:
   ```bash
   make
   ```

### Build specific files
```bash
cmake --build . --target <target_name>
```

Example:
```bash
cmake --build . --target adugo_main
```

### Relaxed Build Mode

If you want to disable warnings for unused parameters and variables, use the relaxed build option:

```bash
cmake -DRELAXED_BUILD=ON ..
cmake --build .
```

## Build Output

All compiled binaries are placed in the `bin/` directory.

### Main Executable

- **`adugo_main`** - The main application (built from `adugo_main.cc`)

### Example Executables

The build system automatically compiles all example files in the `examples/` directory:

- `a_star_example`
- `chess_main`
- `tic_tac_toe_example_pruning`
- `tic_tac_toe_example`
- `tile_main`
- `visual_a_star_example`
- `visual_breadth_example`
- `visual_tic_tac_toe_example`

## Running the Binaries

After building, run any binary from the project root:

```bash
./bin/adugo_main
./bin/chess_main
./bin/tic_tac_toe_example
# ... etc
```

## Generating Documentation

If you have Doxygen installed, you can generate API documentation:

```bash
cd build
make docs
```

This will run Doxygen using the `Doxyfile` configuration and generate documentation in the project directory.

## Adding New Source Files

If you add new `.cc` source files to the project:

1. **For new problems** (in `data_structure/basic_search/problems/`):
   - Add the file path to `BASIC_SEARCH_PROBLEMS_SOURCES` list in `CMakeLists.txt`

2. **For new games** (in `data_structure/adversarial_search/games/`):
   - Add the file path to `ADVERSARIAL_SEARCH_GAMES_SOURCES` list in `CMakeLists.txt`

3. **For new examples** (in `examples/`):
   - No changes needed! Example files are automatically detected and compiled.

4. **Reconfigure and rebuild:**
   ```bash
   cd build
   cmake ..
   cmake --build .
   ```

## Clean Build

To perform a clean build:

```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

## Troubleshooting

**CMake version error:**
- Ensure you have CMake 3.28 or higher: `cmake --version`

**ncurses not found:**
- Install the ncurses development library (see Prerequisites section)

**Compilation errors:**
- Try the relaxed build mode: `cmake -DRELAXED_BUILD=ON ..`
- Ensure your compiler supports C++17
