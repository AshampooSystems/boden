#############################################################################
# Package

set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/package/generator.cmake)

math(EXPR arch "${CMAKE_SIZEOF_VOID_P} * 8")

set(buildType "${CMAKE_BUILD_TYPE}")
if("${buildType}" STREQUAL "")
    set(buildType "%BUILD_TYPE%")
endif()

if(ANDROID_ABI)
    set(arch "${ANDROID_ABI}-${ANDROID_NATIVE_API_LEVEL}")
else()
    set(arch "x${arch}")
endif()

set(CPACK_PACKAGE_FILE_NAME "boden-${PROJECT_VERSION}-${BDN_TARGET}-${arch}-${buildType}-${CMAKE_CXX_COMPILER_ID}")

message(STATUS "Package name: ${CPACK_PACKAGE_FILE_NAME}")

include(CPack)

cpack_add_component_group(boden_root
    DISPLAY_NAME "Boden"
    DESCRIPTION "The boden Framework")

cpack_add_component_group(boden_framework
    DISPLAY_NAME "Framework"
    DESCRIPTION "Contains the boden Framework"
    PARENT_GROUP boden_root)

cpack_add_component_group(boden_tests
    DISPLAY_NAME "Unit tests"
    DESCRIPTION "Contains the boden Framework Unittests"
    PARENT_GROUP boden_root)

cpack_add_component_group(boden_examples
    DISPLAY_NAME "Examples"
    DESCRIPTION "Contains the boden Framework Examples"
    PARENT_GROUP boden_root)

cpack_add_component(library
                    DISPLAY_NAME "Library"
                    DESCRIPTION "The boden framework"
                    REQUIRED
                    INSTALL_TYPES
                        all libonly
                    GROUP boden_framework)

cpack_add_component(tests
                    DISPLAY_NAME "Unit tests"
                    DESCRIPTION "The boden unittests"
                    GROUP boden_tests
                    INSTALL_TYPES
                        all
                    DEPENDS library)

cpack_add_component(examples
                    DISPLAY_NAME "Examples"
                    DESCRIPTION "The boden examples"
                    GROUP boden_examples
                    INSTALL_TYPES
                        all
                    DEPENDS library)

cpack_add_install_type(all DISPLAY_NAME "Everything")
cpack_add_install_type(libonly DISPLAY_NAME "Library Only")


