
if(BDN_PLATFORM_OSX)
    add_subdirectory(mac)
    add_subdirectory(applecommon)
    target_link_libraries(ui INTERFACE ui_mac ui_applecommon)
elseif(BDN_PLATFORM_IOS)
    add_subdirectory(ios)
    add_subdirectory(applecommon)
    target_link_libraries(ui INTERFACE ui_ios ui_applecommon)
elseif(BDN_PLATFORM_ANDROID)
    add_subdirectory(android)
    target_link_libraries(ui INTERFACE ui_android)
endif()


