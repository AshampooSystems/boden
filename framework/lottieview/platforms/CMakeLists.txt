if(BDN_PLATFORM_OSX)
    add_subdirectory(mac)
    target_link_libraries(lottieview INTERFACE lottieview_mac)
elseif(BDN_PLATFORM_IOS)
    add_subdirectory(ios)
    target_link_libraries(lottieview INTERFACE lottieview_ios)
elseif(BDN_PLATFORM_ANDROID)
    add_subdirectory(android)
    target_link_libraries(lottieview INTERFACE lottieview_android)
endif()


