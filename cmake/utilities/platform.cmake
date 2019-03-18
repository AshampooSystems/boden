set(BDN_AVAILABLE_PLATFORMS "mac;ios;android" CACHE STRING "List of possible build targets")

# Auto detect target platform

if( IOS )
    set( BDN_DETECTED_TARGET "ios" )
elseif( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
    set( BDN_DETECTED_TARGET "mac" )
elseif( ANDROID )
    set( BDN_DETECTED_TARGET "android" )
else()
    message(STATUS "Warning, could not autodetect platform!")
endif()

# Select target platform ( auto detect, or use users selection )
if(NOT BDN_TARGET)
    set(BDN_TARGET ${BDN_DETECTED_TARGET})
endif()


# Global options based on selected platform

set(BDN_USES_FK No)
set(BDN_USES_JAVA No)
set(BDN_NEEDS_TO_BE_SHARED_LIBRARY No)
set(BDN_NEEDS_TO_BE_STATIC_LIBRARY No)

set(BDN_PLATFORM_IOS No)
set(BDN_PLATFORM_OSX No)
set(BDN_PLATFORM_ANDROID No)

if(BDN_TARGET STREQUAL "ios")
    set( BDN_USES_FK Yes )
    set( BDN_PLATFORM_FAMILY "posix")
    set( BDN_NEEDS_TO_BE_STATIC_LIBRARY Yes )
    set( BDN_PLATFORM_IOS Yes )
elseif(BDN_TARGET STREQUAL "mac")
    set( BDN_USES_FK Yes )
    set( BDN_PLATFORM_FAMILY "posix")
    set( BDN_PLATFORM_OSX Yes )
elseif(BDN_TARGET STREQUAL "android")
    set( BDN_USES_JAVA Yes )
    set( BDN_PLATFORM_FAMILY "posix")
    set( BDN_NEEDS_TO_BE_SHARED_LIBRARY Yes )
    set( BDN_PLATFORM_ANDROID Yes )
endif()

if( "${BDN_PLATFORM_FAMILY}" STREQUAL "posix" )
    set( BDN_PLATFORM_FAMILY_POSIX Yes )
endif()

message(STATUS "Boden platform:")
message(STATUS "  Name: ${BDN_TARGET}")
message(STATUS "  Family: ${BDN_PLATFORM_FAMILY}")
message(STATUS "Boden configuration:")
message(STATUS "  Uses foundation kit: ${BDN_USES_FK}")
message(STATUS "  Uses java: ${BDN_USES_JAVA}")
message(STATUS "  Force shared: ${BDN_NEEDS_TO_BE_SHARED_LIBRARY}")
message(STATUS "  Force static: ${BDN_NEEDS_TO_BE_STATIC_LIBRARY}")

if(BDN_PLATFORM_OSX)
    if("${CMAKE_OSX_DEPLOYMENT_TARGET}" STREQUAL "")
        message(STATUS "  Mac OS X SDK: <unknown>")
    else()
        get_filename_component(sdk_version ${CMAKE_OSX_SYSROOT} NAME)
        message(STATUS "  Mac OS X SDK: ${sdk_version} (${CMAKE_OSX_SYSROOT})")
    endif()
    
    
    if("${CMAKE_OSX_DEPLOYMENT_TARGET}" STREQUAL "")
        message(STATUS "  Deployment target: <default>")
    else()
        message(STATUS "  Deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
    endif()
endif()

# Make options globally available

set(BDN_TARGET ${BDN_TARGET} CACHE INTERNAL "Boden target name")
set(BDN_PLATFORM_FAMILY ${BDN_PLATFORM_FAMILY} CACHE INTERNAL "Boden target family")
set(BDN_USES_FK ${BDN_USES_FK} CACHE INTERNAL "Boden uses Foundation Kit")
set(BDN_USES_JAVA ${BDN_USES_JAVA} CACHE INTERNAL "Boden uses java")
set(BDN_NEEDS_TO_BE_SHARED_LIBRARY ${BDN_NEEDS_TO_BE_SHARED_LIBRARY} CACHE INTERNAL "boden needs to be a shared library")
set(BDN_NEEDS_TO_BE_STATIC_LIBRARY ${BDN_NEEDS_TO_BE_STATIC_LIBRARY} CACHE INTERNAL "boden needs to be a static library")

set(BDN_PLATFORM_IOS            ${BDN_PLATFORM_IOS} CACHE INTERNAL "")
set(BDN_PLATFORM_OSX            ${BDN_PLATFORM_OSX} CACHE INTERNAL "")
set(BDN_PLATFORM_ANDROID        ${BDN_PLATFORM_ANDROID} CACHE INTERNAL "")

set(BDN_PLATFORM_FAMILY_POSIX   ${BDN_PLATFORM_FAMILY_POSIX} CACHE INTERNAL "")

mark_as_advanced(BDN_PLATFORM_IOS)
mark_as_advanced(BDN_PLATFORM_OSX)
mark_as_advanced(BDN_PLATFORM_ANDROID)

mark_as_advanced(BDN_PLATFORM_FAMILY_POSIX)

set_property(CACHE BDN_TARGET PROPERTY STRINGS ${BDN_AVAILABLE_PLATFORMS})
mark_as_advanced(BDN_AVAILABLE_PLATFORMS)

macro(add_link_type_definitions TARGET)

    if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
        target_compile_definitions(${TARGET} PRIVATE BDN_SHARED_LIB=1)
    elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
        target_compile_definitions(${TARGET} PRIVATE BDN_SHARED_LIB=0)
    endif()

endmacro()

macro(add_universal_library TARGET)
    set(options TIDY)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(_LIB "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )

    if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
        add_library(${TARGET} SHARED ${_LIB_SOURCES})
    elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
        add_library(${TARGET} STATIC ${_LIB_SOURCES})
    endif()

    if(_LIB_TIDY AND BDN_ENABLE_CLANG_TIDY)
        message("Adding tidy target for ${TARGET} (${BDN_CLANG_TIDY_OPTIONS})")
        set_target_properties(${TARGET} PROPERTIES CXX_CLANG_TIDY "${BDN_CLANG_TIDY_OPTIONS}")
    endif()

    add_clangformat(${TARGET})
    add_link_type_definitions(${TARGET})
endmacro()

macro(add_universal_executable TARGET)
    set(options TIDY CONSOLE)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(_APP "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )


    if(BDN_PLATFORM_IOS)
        add_executable(${TARGET} MACOSX_BUNDLE ${_APP_SOURCES} )
    elseif(BDN_PLATFORM_OSX)
        if(${CONSOLE_APP})
            add_executable(${TARGET} ${_APP_SOURCES} )
        else()
            add_executable(${TARGET} MACOSX_BUNDLE ${_APP_SOURCES} )
        endif()
    elseif(BDN_PLATFORM_ANDROID)
        if(${BAUER_RUN})
            add_executable(${TARGET} ${_APP_SOURCES} )
        else()
            add_library(${TARGET} SHARED ${_APP_SOURCES} )
        endif()

        android_manifest(${TARGET})
    else()
        add_executable(${TARGET} ${_APP_SOURCES} )
    endif()

    if(CONSOLE)
        target_compile_definitions(${TARGET} PRIVATE "BDN_COMPILING_COMMANDLINE_APP")
    endif()

    if(_APP_TIDY AND BDN_ENABLE_CLANG_TIDY)
        message("Adding tidy target for ${TARGET} (${BDN_CLANG_TIDY_OPTIONS})")
        set_target_properties(${TARGET} PROPERTIES CXX_CLANG_TIDY "${BDN_CLANG_TIDY_OPTIONS}")
    endif()

    add_clangformat(${TARGET})
    add_link_type_definitions(${TARGET})
endmacro()

macro(add_platform_library )
    set(options DONT_LINK_PARENT_LIBRARY NO_LINT)
    set(oneValueArgs NAME SOURCE_FOLDER COMPONENT_NAME PARENT_LIBRARY)
    set(multiValueArgs)
    cmake_parse_arguments(_PLATFORM_LIB "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )


    set(_library_name ${_PLATFORM_LIB_PARENT_LIBRARY}_${_PLATFORM_LIB_NAME})

    ##########################################################################
    # Files

    file(GLOB_RECURSE _files
        ${_PLATFORM_LIB_SOURCE_FOLDER}/java/*.java
        ${_PLATFORM_LIB_SOURCE_FOLDER}/src/*.cpp
        ${_PLATFORM_LIB_SOURCE_FOLDER}/src/*.c
        ${_PLATFORM_LIB_SOURCE_FOLDER}/src/*.m
        ${_PLATFORM_LIB_SOURCE_FOLDER}/src/*.mm
        ${_PLATFORM_LIB_SOURCE_FOLDER}/src/*.h
        ${_PLATFORM_LIB_SOURCE_FOLDER}/src/*.hh
        ${_PLATFORM_LIB_SOURCE_FOLDER}/include/*.h
        ${_PLATFORM_LIB_SOURCE_FOLDER}/include/*.hh)

    ##########################################################################
    # Target
    if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
        add_library(${_library_name} SHARED ${_files})
    elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
        add_library(${_library_name} STATIC ${_files})
    endif()

    add_clangformat(${_library_name})
    add_link_type_definitions(${_library_name})

    if(NOT _PLATFORM_LIB_DONT_LINK_PARENT_LIBRARY)
        target_link_libraries(${_library_name} PUBLIC ${_PLATFORM_LIB_PARENT_LIBRARY})
    endif()

    if(NOT _PLATFORM_LIB_NO_LINT AND BDN_ENABLE_CLANG_TIDY)
        message("Adding tidy target for ${_library_name} (${BDN_CLANG_TIDY_OPTIONS})")
        set_target_properties(${_library_name} PROPERTIES CXX_CLANG_TIDY "${BDN_CLANG_TIDY_OPTIONS}")
    endif()


    ##########################################################################
    # Includes

    target_include_directories(${_library_name}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${_PLATFORM_LIB_SOURCE_FOLDER}/include>
        )

    group_sources_automatically(${_PLATFORM_LIB_SOURCE_FOLDER} ${_files})

    ##########################################################################
    # Install

    install(TARGETS ${_library_name}
        EXPORT ${_library_name}-export
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT
          ${_PLATFORM_LIB_COMPONENT_NAME})

    install(DIRECTORY ${_PLATFORM_LIB_SOURCE_FOLDER}/include/
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT
          ${_PLATFORM_LIB_COMPONENT_NAME}
        FILES_MATCHING PATTERN
            "*.h")

    install(EXPORT ${_library_name}-export
        FILE
            ${_library_name}Targets.cmake
        NAMESPACE
            Boden::
        DESTINATION
            ${bodenConfigPackageLocation}
        COMPONENT
          ${_PLATFORM_LIB_COMPONENT_NAME})
endmacro()
