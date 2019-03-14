add_platform_library(ios ${CMAKE_CURRENT_LIST_DIR} IOS webview )
target_link_libraries(webview_ios PRIVATE webview_applecommon)
