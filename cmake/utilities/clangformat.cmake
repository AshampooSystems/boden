option(BDN_ALWAYS_CLANG_FORMAT "Run clang-format on every build" OFF)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(CLANG_FORMAT_EXECUTABLE "${CMAKE_SOURCE_DIR}/3rdparty/clang-format/clang-format-darwin" CACHE INTERNAL "")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(CLANG_FORMAT_EXECUTABLE "${CMAKE_SOURCE_DIR}/3rdparty/clang-format/clang-format-${BDN_LINUX_DISTRIBUTION_ID}" CACHE INTERNAL "")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(CLANG_FORMAT_EXECUTABLE "${CMAKE_SOURCE_DIR}/3rdparty/clang-format/clang-format-windows.exe" CACHE INTERNAL "")
else()
    set(CLANG_FORMAT_EXECUTABLE "CLANG_FORMAT_EXECUTABLE-NOTFOUND" CACHE INTERNAL "")
endif()

if(NOT EXISTS ${CLANG_FORMAT_EXECUTABLE})
    set(CLANG_FORMAT_EXECUTABLE "CLANG_FORMAT_EXECUTABLE-NOTFOUND" CACHE INTERNAL "")
endif()

if(${CLANG_FORMAT_EXECUTABLE} STREQUAL "CLANG_FORMAT_EXECUTABLE-NOTFOUND")
    message(STATUS "clang-format was not found!")
endif()

set(CLANG_FORMAT_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Root Dir of this file")

set(BDN_CODE_FORMATTING_TARGETS "" CACHE INTERNAL "")
mark_as_advanced(BDN_CODE_FORMATTING_TARGETS)

function(add_clangformat _targetname)
    list(APPEND BDN_CODE_FORMATTING_TARGETS ${_targetname})
    set(BDN_CODE_FORMATTING_TARGETS ${BDN_CODE_FORMATTING_TARGETS} CACHE INTERNAL "")
endfunction()

function(add_clangformat_target _targetname)
    set(SOURCES ${ARGN})

    if (NOT ${CLANG_FORMAT_EXECUTABLE} STREQUAL "CLANG_FORMAT_EXECUTABLE-NOTFOUND")
        if (NOT TARGET ${_targetname})
            message(FATAL_ERROR "add_clangformat should only be called on targets (got " ${_targetname} ")")
        endif ()
 
        # figure out which sources this should be applied to
        get_target_property(_builddir ${_targetname} BINARY_DIR)

        set(_sources "")
        foreach (_source ${SOURCES})
            if (NOT TARGET ${_source})
                get_filename_component(_source_file ${_source} NAME)
                get_source_file_property(_clang_loc "${_source}" LOCATION)

                string(SHA256 sha ${_clang_loc})
 
                set(_format_file "clang-format/${sha}.format")

                add_custom_command(OUTPUT ${_format_file}
                        DEPENDS ${_source} ${CLANG_FORMAT_ROOT}/../.clang-format
                        COMMENT "Clang-Format ${_source}"
                        COMMAND ${CLANG_FORMAT_EXECUTABLE} -style=file -fallback-style=WebKit -i ${_clang_loc}
                        COMMAND ${CMAKE_COMMAND} -E touch ${_format_file})

                list(APPEND _sources ${_format_file})
            endif ()
        endforeach ()
 
        if (_sources)
            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/clang-format)

            add_custom_target(${_targetname}_clangformat
                    SOURCES ${_sources}
                    COMMENT "Clang-Format for target ${_target}")
 
            add_dependencies(${_targetname} ${_targetname}_clangformat)
        endif ()
    endif()
endfunction()

function(add_global_clangformat TARGETNAME ROOTFOLDER)
    if (NOT ${CLANG_FORMAT_EXECUTABLE} STREQUAL "CLANG_FORMAT_EXECUTABLE-NOTFOUND")

        file(GLOB_RECURSE _ALL_SOURCES
            ${ROOTFOLDER}/*.cpp
            ${ROOTFOLDER}/*.c
            ${ROOTFOLDER}/*.h
            ${ROOTFOLDER}/*.hh
            ${ROOTFOLDER}/*.mm
            )

        list(FILTER _ALL_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/3rdparty")
        list(FILTER _ALL_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/3rdparty_build")
        list(FILTER _ALL_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/build")
        list(FILTER _ALL_SOURCES EXCLUDE REGEX ${CMAKE_BINARY_DIR})

        add_custom_target(${TARGETNAME} COMMENT "clang-format main target")

        add_clangformat_target(${TARGETNAME} ${_ALL_SOURCES})

        add_dependencies(${TARGETNAME} ${TARGETNAME}_clangformat)

        if(NOT BDN_ALWAYS_CLANG_FORMAT)
            set_target_properties(${TARGETNAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
        else()
            foreach(_target ${BDN_CODE_FORMATTING_TARGETS})
                add_dependencies(${_target} ${TARGETNAME})
            endforeach()
        endif()

    endif()
endfunction()





