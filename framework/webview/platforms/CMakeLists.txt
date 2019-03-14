if(BDN_PLATFORM_OSX OR BDN_PLATFORM_IOS)
    add_subdirectory(applecommon)
endif()

if(BDN_PLATFORM_OSX)
    add_subdirectory(mac)
    target_link_libraries(webview INTERFACE webview_mac)
elseif(BDN_PLATFORM_IOS)
    add_subdirectory(ios)
    target_link_libraries(webview INTERFACE webview_ios)
elseif(BDN_PLATFORM_ANDROID)
    add_subdirectory(android)
    target_link_libraries(webview INTERFACE webview_android)
endif()


