
if(BDN_PLATFORM_OSX)
    add_subdirectory(mac)
    target_link_libraries(net INTERFACE net_mac)
elseif(BDN_PLATFORM_IOS)
    add_subdirectory(mac)
    target_link_libraries(net INTERFACE net_mac)
elseif(BDN_PLATFORM_ANDROID)
    add_subdirectory(android)
    target_link_libraries(net INTERFACE net_android)
endif()


