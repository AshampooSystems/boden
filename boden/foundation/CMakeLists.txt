include(CMakeDependentOption)

if(CMAKE_CXX_STANDARD VERSION_LESS 14)
    message(FATAL_ERROR "Boden requires at least the C++14 standard")
endif()

CMAKE_DEPENDENT_OPTION(BODEN_SHARED_LIB "Compile foundation as a shared library" OFF "NOT BODEN_NEEDS_TO_BE_SHARED_LIBRARY;NOT BODEN_NEEDS_TO_BE_STATIC_LIBRARY" Off)

##########################################################################
# Files

add_sources(_BDN_FILES _BDN_HEADERS ./)
add_sources(_BDN_FILES _BDN_HEADERS ./${BODEN_TARGET})
add_sources(_BDN_FILES _BDN_HEADERS ./test)

if( BODEN_USES_FK )
    add_sources(_BDN_FILES _BDN_HEADERS ./fk)
endif()

if( BODEN_USES_JAVA )
    add_sources(_BDN_FILES _BDN_HEADERS ./java)
    file(GLOB_RECURSE _BDN_JAVA_FILES ./src/java/*.cpp ./java/*.java)
    list(APPEND _BDN_FILES ${_BDN_JAVA_FILES})
endif()

if( BODEN_USES_GTK )
    add_sources(_BDN_FILES _BDN_HEADERS ./gtk)
endif()

if(BODEN_USES_PTHREAD)
    add_sources(_BDN_FILES _BDN_HEADERS ./pthread)
endif()

##########################################################################
# Target

if(BODEN_NEEDS_TO_BE_SHARED_LIBRARY OR BODEN_SHARED_LIB)
    set(BODEN_SHARED_LIB Yes)
    add_library(foundation SHARED ${_BDN_FILES})
elseif(BODEN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BODEN_SHARED_LIB)
    set(BODEN_SHARED_LIB No)
    add_library(foundation STATIC ${_BDN_FILES})
endif()

target_include_directories(foundation
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_BDN_FILES})

##########################################################################
# Platform setup

setup_windows_uwp(foundation)
setup_gtk(foundation)
setup_ios(foundation)

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

##########################################################################
# CMake to C++

cmake_variable_to_define(foundation PUBLIC BODEN_USES_PTHREAD)
cmake_variable_to_define(foundation PUBLIC BODEN_USES_FK)
cmake_variable_to_define(foundation PUBLIC BODEN_USES_JAVA)
cmake_variable_to_define(foundation PUBLIC BODEN_USES_GTK)

cmake_variable_to_define(foundation PUBLIC BODEN_TARGET)
cmake_variable_to_define(foundation PUBLIC BODEN_TARGET_FAMILY)

##########################################################################
# Warnings

if(BODEN_USES_GTK)
    disable_deprecated_register_warning(foundation)
endif()

##########################################################################
# Libraries

if(BODEN_TARGET_WIN32)
    target_link_libraries(foundation PUBLIC shcore)
elseif(BODEN_TARGET_MAC)
    target_link_libraries(foundation PUBLIC "-framework Cocoa")
elseif(BODEN_TARGET_IOS)
    target_link_libraries(foundation PUBLIC "-framework UIKit" "-framework Foundation" "-framework CoreGraphics")
elseif(BODEN_TARGET_ANDROID)
    find_library(ANDROID_LOG_LIB log)
    mark_as_advanced(ANDROID_LOG_LIB)
    target_link_libraries(foundation PUBLIC ${ANDROID_LOG_LIB})
elseif(BODEN_TARGET_LINUX)
    target_link_libraries(foundation PUBLIC pthread)
endif()

math(EXPR arch "${CMAKE_SIZEOF_VOID_P} * 8")

message(STATUS "Boden library configuration:")
message(STATUS "  Shared: ${BODEN_SHARED_LIB}")
message(STATUS "  Architecture: ${arch} bit")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")

include(install.cmake)




