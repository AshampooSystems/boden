if(${CPACK_GENERATOR} STREQUAL "IFW")
    include(${CMAKE_CURRENT_LIST_DIR}/ifw.cmake)
else()

endif()

string(REPLACE "%BUILD_TYPE%"
       "${CPACK_BUILD_CONFIG}" CPACK_PACKAGE_FILE_NAME
       "${CPACK_PACKAGE_FILE_NAME}")

# MT: If you want to know what variables are available ...
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    set(_list "${_list}\n${_variableName}=${${_variableName}}")
#endforeach()
#
#message(FATAL_ERROR ${_list})

