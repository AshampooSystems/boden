##########################################################################
# Files

add_sources(_BDN_WEBVIEW_FILES _BDN_HEADERS ./)

GenerateTopLevelIncludeFile(_BDN_WEBVIEW_COMBINED ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/webview-module.h ${CMAKE_CURRENT_LIST_DIR}/include ${_BDN_HEADERS})

set(_BDN_WEBVIEW_FILES ${_BDN_WEBVIEW_FILES} ${_BDN_WEBVIEW_COMBINED})

add_sources(_BDN_WEBVIEW_FILES _BDN_HEADERS ./${BDN_TARGET})

if( BDN_USES_FK )
    add_sources(_BDN_WEBVIEW_FILES _BDN_HEADERS ./fk)
endif()

if( BDN_USES_JAVA )
    add_sources(_BDN_WEBVIEW_FILES _BDN_HEADERS ./java)
    file(GLOB_RECURSE _BDN_JAVA_FILES ./src/java/*.cpp ./java/*.java)
    list(APPEND _BDN_WEBVIEW_FILES ${_BDN_JAVA_FILES})
endif()

list(APPEND _BDN_WEBVIEW_FILES)

add_universal_library(webview ${_BDN_WEBVIEW_FILES})

target_link_libraries(webview PUBLIC foundation ui)
target_include_directories(webview
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_include_directories(webview PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

include(install.cmake)

add_subdirectory(platforms)

target_link_libraries(webview INTERFACE ${BDN_WEBVIEW_PLATFORM_LIBRARIES})
add_library(Boden::WebView ALIAS webview)
target_link_libraries(Boden_All INTERFACE webview)
