add_sources(_BDN_LOTTIEVIEW_FILES _BDN_LOTTIEVIEW_HEADERS ./)

GenerateTopLevelIncludeFile(_BDN_LOTTIEVIEW_COMBINED
    ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/ui/lottie.h
    ${CMAKE_CURRENT_LIST_DIR}/include/
    ${_BDN_LOTTIEVIEW_HEADERS})

list(APPEND _BDN_LOTTIEVIEW_FILES
    ${_BDN_LOTTIEVIEW_COMBINED}
    ${_BDN_LOTTIEVIEW_HEADERS})

add_universal_library(lottieview TIDY SOURCES ${_BDN_LOTTIEVIEW_FILES})

target_link_libraries(lottieview PUBLIC Boden::UI)
target_include_directories(lottieview
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_include_directories(lottieview PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

include(install.cmake)

add_subdirectory(platforms)

add_library(Boden::LottieView ALIAS lottieview)
target_link_libraries(Boden_All INTERFACE lottieview)


