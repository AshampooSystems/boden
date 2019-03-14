add_platform_library(applecommon ${CMAKE_CURRENT_LIST_DIR} IOS-MAC webview NO)
target_link_libraries(webview_applecommon PRIVATE ui)
