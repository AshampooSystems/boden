cmake_minimum_required(VERSION 3.6)

project(boden)

set( CMAKE_CXX_STANDARD 14 )

enable_testing()

# cmake utilities
add_subdirectory(cmake)
add_subdirectory(bauer)

setup_web_ems(boden)

# Main Library
add_subdirectory(boden)

# Examples
option(BODEN_BUILD_EXAMPLES "Build boden examples" ON)

if(BODEN_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif(BODEN_BUILD_EXAMPLES)

# Tests
option(BODEN_BUILD_TESTS "Build boden tests" ON)

if(BODEN_BUILD_TESTS)
	add_subdirectory(tests)
endif(BODEN_BUILD_TESTS)
