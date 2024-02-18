cmake_minimum_required(VERSION 3.5.0)
project(nbtpp VERSION 1.0.0 DESCRIPTION "A c++ library for interacting with minecraft NBT files")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NBTPP_EXAMPLES "Build nbtpp examples" OFF)
option(NBTPP_ZLIB "Build nbtpp with zlib support for compressed files" ON)

if (${NBTPP_ZLIB})
    include(cmake/CPM.cmake)
    CPMAddPackage("gh:madler/zlib#v1.3.1")
endif()

file(GLOB SOURCES
    src/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)
if (${NBTPP_ZLIB})
    target_compile_definitions(${PROJECT_NAME} PUBLIC nbtpp_zlib)
    target_link_libraries(${PROJECT_NAME} PRIVATE zlibstatic)
endif()

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR} OR ${NBTPP_EXAMPLES})
    add_subdirectory(examples)
endif()