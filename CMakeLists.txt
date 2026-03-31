cmake_minimum_required(VERSION 3.15) 
project(SolarSystem VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dependencies
add_subdirectory(external)

# Modules
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(app)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(CMAKE_CXX_CLANG_TIDY
        ${CLANG_TIDY_EXE};
        --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;
        -p=${CMAKE_BINARY_DIR}
    )
else()
    message(WARNING "clang-tidy not found, skipping checks")
endif()