include(CMakeDependentOption)

if(CMAKE_CXX_STANDARD VERSION_LESS 17)
    message(FATAL_ERROR "Boden requires at least the C++17 standard")
endif()

CMAKE_DEPENDENT_OPTION(BDN_SHARED_LIB "Compile foundation as a shared library" OFF "NOT BDN_NEEDS_TO_BE_SHARED_LIBRARY;NOT BDN_NEEDS_TO_BE_STATIC_LIBRARY" Off)

##########################################################################
# Files

file(GLOB_RECURSE _files
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/src/*.m
    ${CMAKE_CURRENT_LIST_DIR}/src/*.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/*.h
    ${CMAKE_CURRENT_LIST_DIR}/src/*.hh
    ${CMAKE_CURRENT_LIST_DIR}/include/*.h
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hh)

file(GLOB_RECURSE _headers ${CMAKE_CURRENT_LIST_DIR}/include/*.h)

GenerateTopLevelIncludeFile(_combined_header
    ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/foundation.h
    ${CMAKE_CURRENT_LIST_DIR}/include ${_headers} ${CMAKE_CURRENT_LIST_DIR}/include/bdn/appInit.h)

configure_file(${CMAKE_CURRENT_LIST_DIR}/config/platform.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/platform.h)

list(APPEND _files ${CMAKE_CURRENT_LIST_DIR}/config/platform.h.in)

set(_BDN_GENERATED_FILES ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/platform.h ${_combined_header})

##########################################################################
# Target

add_universal_library(foundation TIDY SOURCES ${_files} ${_BDN_GENERATED_FILES})

target_include_directories(foundation
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_include_directories(foundation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_BDN_FILES})

##########################################################################
# Options

enable_warnings_as_errors(foundation PUBLIC)
enable_automatic_reference_counting(foundation PUBLIC)
enable_big_object_files(foundation PUBLIC)
enable_unicode(foundation PUBLIC)
enable_multicore_build(foundation PUBLIC)

# MT: I think we should enable this ( gcc on linux )
# but a lot of errors are generated from it atm.
#enable_override_warning(foundation PUBLIC)

math(EXPR arch "${CMAKE_SIZEOF_VOID_P} * 8")

message(STATUS "Boden library configuration:")
message(STATUS "  Shared: ${BDN_SHARED_LIB}")
message(STATUS "  Architecture: ${arch} bit")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")

include(install.cmake)

add_subdirectory(platforms)

target_link_libraries(foundation INTERFACE ${BDN_FOUNDATION_PLATFORM_LIBRARIES})
add_library(Boden::Foundation ALIAS foundation)
target_link_libraries(Boden_All INTERFACE foundation)

