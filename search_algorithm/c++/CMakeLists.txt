cmake_minimum_required(VERSION 3.28)
project(AI)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags
add_compile_options(-Wall -Wextra -Wpedantic)

# Relaxed build
option(RELAXED_BUILD "Relaxed warning flags" OFF)
if (RELAXED_BUILD)
message(STATUS "Relaxed build activated")
add_compile_options(-Wno-unused-parameter -Wno-unused-variable)
endif()

# Out directory of binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

##### IF YOU ADD A NEW SOURCE FILE, ADD IT TO THE RESPECTIVE LIST BELOW ########
################## OR CREATE NEW LISTS IF NECESSARY ############################
# Problem sources (.cc)
set(BASIC_SEARCH_PROBLEMS_SOURCES
    "data_structure/basic_search/problems/sliding_tile_problem.cc"
    "data_structure/basic_search/problems/chess_board_problem.cc"
)

# Game sources (.cc)
set(ADVERSARIAL_SEARCH_GAMES_SOURCES
    "data_structure/adversarial_search/games/tic_tac_toe_game.cc")

### Libraries ###
add_library(problems STATIC ${BASIC_SEARCH_PROBLEMS_SOURCES})
target_include_directories(problems PUBLIC ${CMAKE_SOURCE_DIR})

add_library(games STATIC ${ADVERSARIAL_SEARCH_GAMES_SOURCES})
target_include_directories(games PUBLIC ${CMAKE_SOURCE_DIR})

# ncurses
find_package(Curses REQUIRED)
if (NOT Curses_FOUND)
    message(FATAL_ERROR "Curses library not found!")
endif()

### Targets ###
# Main target
set(MAIN_EXECUTABLE "tile_main")
set (MAIN_SOURCE "tile_main.cc")    
add_executable(${MAIN_EXECUTABLE} ${MAIN_SOURCE})
target_link_libraries(${MAIN_EXECUTABLE} PRIVATE problems games ${CURSES_LIBRARIES})

# Examples target
file(GLOB EXAMPLE_SOURCES "examples/*.cc")
foreach(example_file ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_file} NAME_WE)
    add_executable(${example_name} ${example_file})
    target_link_libraries(${example_name} PRIVATE problems ${CURSES_LIBRARIES})
endforeach()


# Doxygen documentation
find_program(DOXYGEN doxygen)
if (DOXYGEN)
    add_custom_target(docs
        COMMAND ${DOXYGEN} Doxyfile WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
else()
    message(STATUS "Doxygen not found, unable to generate documentation")
endif()
    

