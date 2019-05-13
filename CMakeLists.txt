cmake_minimum_required(VERSION 3.10)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./ )

option(BDN_BUILD_TESTS "Build boden tests" ON)
option(BDN_BUILD_EXAMPLES "Build boden examples" ON)

if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW) # Remove when minimum reaches 3.13
endif()

project(boden VERSION 0.2.0 LANGUAGES C CXX)

include(GNUInstallDirs)

set( CMAKE_POSITION_INDEPENDENT_CODE ON )

add_library(Boden_All INTERFACE)
add_library(Boden::All ALIAS Boden_All)

enable_testing()

# cmake utilities
add_subdirectory(cmake)
add_subdirectory(bauer)
add_subdirectory(roger)

fix_ios_package()

# 3rdParty support libraries
add_subdirectory(3rdparty)

# Clang-tidy
option(BDN_ENABLE_CLANG_TIDY "Enable clang-tidy during build" OFF)
set(BDN_CLANG_TIDY_EXECUTABLE "/usr/local/opt/llvm/bin/clang-tidy" CACHE STRING "The path to the clang-tidy executable")

set(BDN_CLANG_TIDY_EXTRA_OPTIONS "" CACHE STRING "Extra options for clang-tidy")
set(BDN_CLANG_TIDY_OPTIONS ${BDN_CLANG_TIDY_EXECUTABLE} ${BDN_CLANG_TIDY_EXTRA_OPTIONS} )

# Main Library
add_subdirectory(framework)

# Examples

if(BDN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(BDN_BUILD_EXAMPLES)

# Tests

if(BDN_BUILD_TESTS)
    add_subdirectory(tests)
endif()

add_global_clangformat(FormatSources ${CMAKE_CURRENT_LIST_DIR})

include(package/package.cmake)
