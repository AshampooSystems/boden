add_platform_library(mac ${CMAKE_CURRENT_LIST_DIR} MacOSX webview )

target_link_libraries(webview_mac PRIVATE webview_applecommon)
