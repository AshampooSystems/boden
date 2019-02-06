macro(group_sources_automatically ROOT)
	if(${CMAKE_VERSION} VERSION_GREATER 3.8.2)
		source_group(TREE ${ROOT} FILES ${ARGN})
	endif()
endmacro()

macro(GenerateTopLevelIncludeFile OUTPUT NAME BASEDIR)
    get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)

    foreach(_file ${ARGN})
        file(RELATIVE_PATH _relativePath ${BASEDIR} ${_file})
        set(_HEADERS_ "${_HEADERS_}#include <${_relativePath}>\n")
    endforeach()

    configure_file(${BODEN_CMAKE_SOURCE_DIR}/templates/combined_header.h.in ${NAME})

    set(${OUTPUT} ${NAME})
endmacro()
