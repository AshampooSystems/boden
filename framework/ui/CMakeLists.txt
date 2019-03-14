
##########################################################################
# Files

add_sources(_BDN_UI_FILES _BDN_HEADERS ./)

GenerateTopLevelIncludeFile(_combined_header ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/ui.h ${CMAKE_CURRENT_LIST_DIR}/include ${_BDN_HEADERS})
set(_BDN_UI_FILES ${_BDN_UI_FILES} ${_combined_header})

add_sources(_BDN_UI_FILES _BDN_HEADERS ./${BDN_TARGET})

if( BDN_USES_FK )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./fk)
endif()

if( BDN_USES_JAVA )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./java)
    file(GLOB_RECURSE _BDN_JAVA_FILES ./src/java/*.cpp ./java/*.java)
    list(APPEND _BDN_UI_FILES ${_BDN_JAVA_FILES})
endif()

add_universal_library(ui ${_BDN_UI_FILES})

target_link_libraries(ui PUBLIC foundation)
target_include_directories(ui
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_include_directories(ui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

include(install.cmake)

add_subdirectory(platforms)

target_link_libraries(ui INTERFACE ${BDN_UI_PLATFORM_LIBRARIES})
