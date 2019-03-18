add_platform_library(NAME ios SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME IOS PARENT_LIBRARY webview )
target_link_libraries(webview_ios PRIVATE webview_applecommon)
