##########################################################################
# Files

file(GLOB_RECURSE _BDN_HEADERS ./include/*.h)
file(GLOB_RECURSE _BDN_SOURCES ./src/*.cpp)

set(_BDN_LAYOUT_FILES ${_BDN_SOURCES} ${_BDN_HEADERS})

add_universal_library(layout TIDY SOURCES ${_BDN_LAYOUT_FILES})

target_link_libraries(layout PUBLIC foundation)
target_include_directories(layout
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_include_directories(layout PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

include(install.cmake)

target_link_libraries(Boden_All INTERFACE layout)
add_library(Boden::Layout ALIAS layout)

