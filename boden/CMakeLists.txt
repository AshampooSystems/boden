include(CMakeDependentOption)

if(CMAKE_CXX_STANDARD VERSION_LESS 14)
	message(FATAL_ERROR "Boden requires at least the C++14 standard")
endif()

CMAKE_DEPENDENT_OPTION(BODEN_SHARED_LIB "Compile boden as a shared library" OFF "NOT BODEN_NEEDS_TO_BE_SHARED_LIBRARY;NOT BODEN_NEEDS_TO_BE_STATIC_LIBRARY" Off)

##########################################################################
# Utilities

function(sourcefiles_from_folder HEADERS SOURCES FOLDER)
    file(GLOB _HEADERS ./include/bdn/${FOLDER}/*.h ./include/bdn/${FOLDER}/*.hh)
    file(GLOB _SOURCES ./src/${FOLDER}/*.cpp ./src/${FOLDER}/*.c ./src/${FOLDER}/*.mm ./src/${FOLDER}/*.m)
    
    set(${HEADERS} ${_HEADERS} PARENT_SCOPE)
    set(${SOURCES} ${_SOURCES} PARENT_SCOPE)
endfunction()

function(add_sources FILES FOLDER)
    sourcefiles_from_folder(_HEADERS _SOURCES ${FOLDER})
    set(${FILES} ${${FILES}} ${_HEADERS} ${_SOURCES} PARENT_SCOPE)
endfunction()

##########################################################################
# Files

add_sources(_BDN_FILES ./)
add_sources(_BDN_FILES ./${BODEN_TARGET})
add_sources(_BDN_FILES ./test)

if( BODEN_USES_FK )
    add_sources(_BDN_FILES ./fk)
endif()

if( BODEN_USES_JAVA )
    add_sources(_BDN_FILES ./java)
    file(GLOB_RECURSE _BDN_JAVA_FILES ./src/java/*.cpp ./java/*.java)
    list(APPEND _BDN_FILES ${_BDN_JAVA_FILES})
endif()

if( BODEN_USES_GTK )
    add_sources(_BDN_FILES ./gtk)
endif()

if(BODEN_USES_PTHREAD)
    add_sources(_BDN_FILES ./pthread)
endif()

##########################################################################
# Target

if(BODEN_NEEDS_TO_BE_SHARED_LIBRARY OR BODEN_SHARED_LIB)
    set(BODEN_SHARED_LIB Yes)
    add_library(boden SHARED ${_BDN_FILES})
elseif(BODEN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BODEN_SHARED_LIB)
    set(BODEN_SHARED_LIB No)
    add_library(boden STATIC ${_BDN_FILES})
endif()

target_include_directories(boden PUBLIC "./include")

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_BDN_FILES})


##########################################################################
# Platform setup

setup_windows_uwp(boden)
setup_gtk(boden)
setup_ios(boden)

##########################################################################
# Options

enable_warnings_as_errors(boden PUBLIC)
enable_automatic_reference_counting(boden PUBLIC)
enable_big_object_files(boden PUBLIC)
enable_unicode(boden PUBLIC)
enable_multicore_build(boden PUBLIC)

##########################################################################
# Architecture

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BODEN_ARCH_64 Yes)
	set(BODEN_ARCH_32 No)
    target_compile_definitions(boden PUBLIC BODEN_ARCH_64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(BODEN_ARCH_64 No)
	set(BODEN_ARCH_32 Yes)
    target_compile_definitions(boden PUBLIC BODEN_ARCH_32)
else()
	message(FATAL_ERROR "Could not deduce 32/64 bit architecture")
endif()

##########################################################################
# Warnings

if(BODEN_USES_GTK)
    disable_deprecated_register_warning(boden)
endif()

##########################################################################
# Libraries

if(BODEN_TARGET_WIN32)
	target_link_libraries(boden PUBLIC shcore)
elseif(BODEN_TARGET_MAC)
    target_link_libraries(boden PUBLIC "-framework Cocoa")
elseif(BODEN_TARGET_IOS)
    target_link_libraries(boden PUBLIC "-framework UIKit" "-framework Foundation" "-framework CoreGraphics")
elseif(BODEN_TARGET_ANDROID)
    find_library(ANDROID_LOG_LIB log)
    mark_as_advanced(ANDROID_LOG_LIB)
    target_link_libraries(boden PUBLIC ${ANDROID_LOG_LIB})
elseif(BODEN_TARGET_LINUX)
    target_link_libraries(boden PUBLIC pthread)
endif()

message(STATUS "Boden library configuration:")
message(STATUS "  Shared: ${BODEN_SHARED_LIB}")
message(STATUS "  32 bit: ${BODEN_ARCH_32}")
message(STATUS "  64 bit: ${BODEN_ARCH_64}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")




