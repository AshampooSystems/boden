set(CPACK_IFW_PACKAGE_TITLE "Boden Framework")
set(CPACK_IFW_PACKAGE_PUBLISHER "Personizer")
set(CPACK_IFW_PRODUCT_URL "http://www.personizer.com")
set(CPACK_IFW_TARGET_DIRECTORY "/tmp/boden-${CPACK_PACKAGE_VERSION}")
set(CPACK_IFW_PACKAGE_GROUP boden)


#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    set(vars "${vars}\n${_variableName}=${${_variableName}}")
#endforeach()
#
#message(FATAL_ERROR "XX: ${vars}")

include(CPackIFW)

# IFW specific
cpack_ifw_configure_component_group(boden_framework
    DEFAULT True)

cpack_ifw_configure_component_group(boden_tests
    DEPENDS "boden_root.boden_framework")

cpack_ifw_configure_component_group(boden_examples
    DEPENDS "boden_root.boden_framework")
