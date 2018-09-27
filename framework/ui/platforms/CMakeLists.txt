
if(BDN_PLATFORM_OSX)
    add_subdirectory(mac)
    target_link_libraries(ui INTERFACE ui_mac)
elseif(BDN_PLATFORM_IOS)
    add_subdirectory(ios)
    target_link_libraries(ui INTERFACE ui_ios)
elseif(BDN_PLATFORM_ANDROID)
    add_subdirectory(android)
    target_link_libraries(ui INTERFACE ui_android)
endif()


