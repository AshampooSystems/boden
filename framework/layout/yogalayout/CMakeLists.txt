##########################################################################
# Files

#add_sources(_BDN_YOGA_LAYOUT_FILES _BDN_HEADERS ./)

file(GLOB_RECURSE _BDN_HEADERS ./include/*.h)
file(GLOB_RECURSE _BDN_SOURCES ./src/*.cpp)

GenerateTopLevelIncludeFile(_BDN_YOGA_LAYOUT_COMBINED ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/yogalayout.h ${CMAKE_CURRENT_LIST_DIR}/include ${_BDN_HEADERS})

set(_BDN_YOGA_LAYOUT_FILES ${_BDN_SOURCES} ${_BDN_HEADERS} ${_BDN_YOGA_LAYOUT_COMBINED})

add_universal_library(yogalayout TIDY SOURCES ${_BDN_YOGA_LAYOUT_FILES})

target_link_libraries(yogalayout PUBLIC ui yogacore)
target_include_directories(yogalayout
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_include_directories(yogalayout PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

#include(install.cmake)

target_link_libraries(Boden_All INTERFACE yogalayout)
add_library(Boden::YogaLayout ALIAS yogalayout)

