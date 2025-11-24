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
    "data_structure/adversarial_search/games/tic_tac_toe_game.cc"
    "data_structure/adversarial_search/games/adugo_game.cc"
)

# Server/Tabuleiro sources (.c)
set(SERVER_TABULEIRO_SOURCES
    "jogo_da_onca/server/tabuleiro.c"
)

# Tabuleiro wrapper (.cc)
set(SERVER_TABULEIRO_WRAPPER_SOURCES
    "jogo_da_onca/tabuleiro_wrapper.cc"
)

### Libraries ###
add_library(problems STATIC ${BASIC_SEARCH_PROBLEMS_SOURCES})
target_include_directories(problems PUBLIC ${CMAKE_SOURCE_DIR})

add_library(games STATIC ${ADVERSARIAL_SEARCH_GAMES_SOURCES})
target_include_directories(games PUBLIC ${CMAKE_SOURCE_DIR})

add_library(tabuleiro STATIC ${SERVER_TABULEIRO_SOURCES})
target_include_directories(tabuleiro PUBLIC ${CMAKE_SOURCE_DIR})

add_library(tabuleiro_wrapper STATIC ${SERVER_TABULEIRO_WRAPPER_SOURCES})
target_include_directories(tabuleiro_wrapper PUBLIC ${CMAKE_SOURCE_DIR})

# ncurses
find_package(Curses REQUIRED)
if (NOT Curses_FOUND)
    message(FATAL_ERROR "Curses library not found!")
endif()

# hiredis - Find the library
find_library(HIREDIS_LIB hiredis)
if (NOT HIREDIS_LIB)
    message(FATAL_ERROR "hiredis library not found! Install it with: sudo apt-get install libhiredis-dev")
endif()

### Targets ###
# Main target
set(MAIN_EXECUTABLE "adugo_main")
set (MAIN_SOURCE "jogo_da_onca/adugo_main.cc")    
add_executable(${MAIN_EXECUTABLE} ${MAIN_SOURCE})
target_link_libraries(${MAIN_EXECUTABLE} PRIVATE problems games tabuleiro tabuleiro_wrapper ${CURSES_LIBRARIES} ${HIREDIS_LIB})
add_dependencies(${MAIN_EXECUTABLE} tabuleiro)

# Examples target
file(GLOB EXAMPLE_SOURCES "examples/*.cc")
foreach(example_file ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_file} NAME_WE)
    add_executable(${example_name} ${example_file})
    target_link_libraries(${example_name} PRIVATE problems games tabuleiro tabuleiro_wrapper ${CURSES_LIBRARIES})
endforeach()

# Test targets
file(GLOB TEST_SOURCES "tests/*_test.cc")
foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE problems games)
endforeach()

# Server makefile target
add_custom_target(server
    COMMAND $(MAKE) -C ${CMAKE_SOURCE_DIR}/jogo_da_onca/server
    COMMENT "Building server components (controlador, exemplo1, exemplo2)" VERBATIM
)

# Clean server target
add_custom_target(clean-server
    COMMAND $(MAKE) -C ${CMAKE_SOURCE_DIR}/jogo_da_onca/server clean
    COMMENT "Cleaning server components" VERBATIM
)

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


