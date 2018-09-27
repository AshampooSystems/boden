
if(BDN_USES_FK)
    add_subdirectory(foundationkit)
endif()

if(BDN_PLATFORM_OSX)
    add_subdirectory(mac)
    target_link_libraries(foundation INTERFACE foundation_mac)
elseif(BDN_PLATFORM_IOS)
    add_subdirectory(ios)
    target_link_libraries(foundation INTERFACE foundation_ios)
elseif(BDN_PLATFORM_ANDROID)
    add_subdirectory(android)
    target_link_libraries(foundation INTERFACE foundation_android)
endif()


