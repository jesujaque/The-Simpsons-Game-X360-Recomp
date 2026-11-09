cmake_minimum_required(VERSION 3.24)

project(
    SimpsonsGameRecomp
    VERSION 0.1.0
    DESCRIPTION "Static recompilation of The Simpsons Game (Xbox 360) for PC"
    HOMEPAGE_URL "https://github.com/jesujaque/The-Simpsons-Game-X360-Recomp"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(SIMPSONS_FULL_BUILD "Build the full ReXGlue game executable (requires SDK + generated code)" OFF)
option(SIMPSONS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
    if(SIMPSONS_WARNINGS_AS_ERRORS)
        add_compile_options(/WX)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(SIMPSONS_WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()
endif()

if(SIMPSONS_FULL_BUILD)
    include(ReXGlue)
endif()

add_subdirectory(game)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

message(STATUS "SimpsonsGameRecomp ${PROJECT_VERSION}")
message(STATUS "  Full build: ${SIMPSONS_FULL_BUILD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
