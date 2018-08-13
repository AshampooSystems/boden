include(CMakeDependentOption)

if(CMAKE_CXX_STANDARD VERSION_LESS 14)
    message(FATAL_ERROR "Boden requires at least the C++14 standard")
endif()

CMAKE_DEPENDENT_OPTION(BDN_SHARED_LIB "Compile foundation as a shared library" OFF "NOT BDN_NEEDS_TO_BE_SHARED_LIBRARY;NOT BDN_NEEDS_TO_BE_STATIC_LIBRARY" Off)

##########################################################################
# Files

add_sources(_BDN_FILES _BDN_HEADERS ./)
add_sources(_BDN_FILES _BDN_HEADERS ./${BDN_TARGET})
add_sources(_BDN_FILES _BDN_HEADERS ./test)

if( BDN_USES_FK )
    add_sources(_BDN_FILES _BDN_HEADERS ./fk)
endif()

if( BDN_USES_JAVA )
    add_sources(_BDN_FILES _BDN_HEADERS ./java)
    file(GLOB_RECURSE _BDN_JAVA_FILES ./src/java/*.cpp ./java/*.java)
    list(APPEND _BDN_FILES ${_BDN_JAVA_FILES})
endif()

if( BDN_USES_GTK )
    add_sources(_BDN_FILES _BDN_HEADERS ./gtk)
endif()

if(BDN_USES_PTHREAD)
    add_sources(_BDN_FILES _BDN_HEADERS ./pthread)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/config/platform.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/platform.h)

list(APPEND _BDN_FILES ${CMAKE_CURRENT_LIST_DIR}/config/platform.h.in)

set(_BDN_GENERATED_FILES ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/platform.h)

##########################################################################
# Target

if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
    set(BDN_SHARED_LIB Yes)
    add_library(foundation SHARED ${_BDN_FILES} ${_BDN_GENERATED_FILES})
elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
    set(BDN_SHARED_LIB No)
    add_library(foundation STATIC ${_BDN_FILES} ${_BDN_GENERATED_FILES})
endif()

add_clangformat(foundation)

target_include_directories(foundation
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_include_directories(foundation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

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
# Warnings

if(BDN_USES_GTK)
    disable_deprecated_register_warning(foundation)
endif()

##########################################################################
# Libraries

if(BDN_PLATFORM_WIN32)
    target_link_libraries(foundation PUBLIC shcore)
elseif(BDN_PLATFORM_OSX)
    target_link_libraries(foundation PUBLIC "-framework Cocoa")
elseif(BDN_PLATFORM_IOS)
    target_link_libraries(foundation PUBLIC "-framework UIKit" "-framework Foundation" "-framework CoreGraphics")
elseif(BDN_PLATFORM_ANDROID)
    find_library(ANDROID_LOG_LIB log)
    mark_as_advanced(ANDROID_LOG_LIB)
    target_link_libraries(foundation PUBLIC ${ANDROID_LOG_LIB})
elseif(BDN_PLATFORM_LINUX)
    target_link_libraries(foundation PUBLIC pthread)
endif()

math(EXPR arch "${CMAKE_SIZEOF_VOID_P} * 8")

message(STATUS "Boden library configuration:")
message(STATUS "  Shared: ${BDN_SHARED_LIB}")
message(STATUS "  Architecture: ${arch} bit")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")

include(install.cmake)




