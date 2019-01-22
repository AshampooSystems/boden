cmake_minimum_required(VERSION 3.13)

project(boden VERSION 0.1.1 LANGUAGES CXX)

include(GNUInstallDirs)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

enable_testing()

# cmake utilities
add_subdirectory(cmake)
add_subdirectory(bauer)

fix_ios_package()
# Documentation
add_subdirectory(doxygen)

# 3rdParty support libraries
add_subdirectory(3rdparty)

# Main Library
add_subdirectory(framework)

# Examples
option(BDN_BUILD_EXAMPLES "Build boden examples" ON)

if(BDN_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif(BDN_BUILD_EXAMPLES)

# Tests
option(BDN_BUILD_TESTS "Build boden tests" ON)

if(BDN_BUILD_TESTS)
	add_subdirectory(tests)
endif(BDN_BUILD_TESTS)


add_global_clangformat(FormatSources ${CMAKE_CURRENT_LIST_DIR})

include(package/package.cmake)

