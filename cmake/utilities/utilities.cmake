macro(group_sources_automatically ROOT)
	if(${CMAKE_VERSION} VERSION_GREATER 3.8.2)
		source_group(TREE ${ROOT} FILES ${ARGN})
	endif()
endmacro()

macro(GenerateTopLevelIncludeFile OUTPUT NAME BASEDIR)
    get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)

    set(_HEADERS_)
    
    foreach(_file ${ARGN})
        file(RELATIVE_PATH _relativePath ${BASEDIR} ${_file})
        if(NOT "${_relativePath}" MATCHES "jni.h")
            set(_HEADERS_ "${_HEADERS_}#include <${_relativePath}>\n")
        endif()
    endforeach()

    configure_file(${BODEN_CMAKE_SOURCE_DIR}/templates/combined_header.h.in ${NAME})

    set(${OUTPUT} ${NAME})
endmacro()

macro(set_target_version )
    set(options )
    set(oneValueArgs TARGET VERSION SHORTVERSION LONGVERSION ANDROID_VERSION_ID)
    set(multiValueArgs )
    cmake_parse_arguments(_ARGUMENTS "${options}" "${oneValueArgs}"
        "${multiValueArgs}" ${ARGN} )

    if(BDN_PLATFORM_IOS)
        set_target_properties(${_ARGUMENTS_TARGET} PROPERTIES
            MACOSX_BUNDLE_LONG_VERSION_STRING "${_ARGUMENTS_LONGVERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${_ARGUMENTS_SHORTVERSION}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${_ARGUMENTS_VERSION}"
            VERSION "${_ARGUMENTS_VERSION}")
    elseif(BDN_PLATFORM_ANDROID)
        target_compile_definitions(${_ARGUMENTS_TARGET} PRIVATE
            -DANDROID_VERSION_ID=${_ARGUMENTS_ANDROID_VERSION_ID}
            -DANDROID_VERSION=${_ARGUMENTS_VERSION})
    endif()
endmacro()
