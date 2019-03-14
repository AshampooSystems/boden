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
    if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
        # Static libs don't work well on android at the moment due to problems in build.py with dependency generation
        add_library(${TARGET} SHARED ${ARGN})
    elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
        add_library(${TARGET} ${ARGN})
    endif()
    add_clangformat(${TARGET})
    add_link_type_definitions(${TARGET})
endmacro()

macro(add_universal_executable TARGET CONSOLE_APP)
    if(BDN_PLATFORM_IOS)
        add_executable(${TARGET} MACOSX_BUNDLE ${ARGN} )
    elseif(BDN_PLATFORM_OSX)
        if(${CONSOLE_APP})
            add_executable(${TARGET} ${ARGN} )
        else()
            add_executable(${TARGET} MACOSX_BUNDLE ${ARGN} )
        endif()
    elseif(BDN_PLATFORM_ANDROID)
        if(${BAUER_RUN})
            add_executable(${TARGET} ${ARGN} )
        else()
            add_library(${TARGET} SHARED ${ARGN} )
        endif()

        android_manifest(${TARGET})

    else()
        add_executable(${TARGET} ${ARGN} )
    endif()

    if(${CONSOLE_APP})
        target_compile_definitions(${TARGET} PRIVATE "BDN_COMPILING_COMMANDLINE_APP")
    endif()
    add_clangformat(${TARGET})
    add_link_type_definitions(${TARGET})
endmacro()

macro(add_platform_library PLATFORM_LIBRARY_NAME SOURCE_FOLDER COMPONENT_NAME PARENT_LIBRARY)
    set(_library_name ${PARENT_LIBRARY}_${PLATFORM_LIBRARY_NAME})
    set(_link_parent YES)

    if(${ARGC} GREATER 4)
        set(_link_parent ${ARGV4})
    endif()

    ##########################################################################
    # Files

    file(GLOB_RECURSE _files
        ${SOURCE_FOLDER}/java/*.java
        ${SOURCE_FOLDER}/src/*.cpp
        ${SOURCE_FOLDER}/src/*.c
        ${SOURCE_FOLDER}/src/*.m
        ${SOURCE_FOLDER}/src/*.mm
        ${SOURCE_FOLDER}/src/*.h
        ${SOURCE_FOLDER}/src/*.hh
        ${SOURCE_FOLDER}/include/*.h
        ${SOURCE_FOLDER}/include/*.hh)

    ##########################################################################
    # Target
    if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
        add_library(${_library_name} SHARED ${_files} ${_BDN_GENERATED_FILES})
    elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
        add_library(${_library_name} STATIC ${_files} ${_BDN_GENERATED_FILES})
    endif()

    add_clangformat(${_library_name})
    add_link_type_definitions(${_library_name})

    if(_link_parent)
        target_link_libraries(${_library_name} PUBLIC ${PARENT_LIBRARY})
    endif()

    ##########################################################################
    # Includes

    target_include_directories(${_library_name}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${SOURCE_FOLDER}/include>
        )

    group_sources_automatically(${SOURCE_FOLDER} ${_files})

    ##########################################################################
    # Install

    install(TARGETS ${_library_name}
        EXPORT ${_library_name}-export
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT
          ${COMPONENT_NAME})

    install(DIRECTORY ${SOURCE_FOLDER}/include/
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT
          ${COMPONENT_NAME}
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
          ${COMPONENT_NAME})
endmacro()
