
set(ComponentName Library)

if(BDN_PLATFORM_OSX)

    install(TARGETS airbnb_lottie_mac
        EXPORT airbnb_lottie_mac-export
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT
          ${ComponentName})

    install(DIRECTORY include/
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT
          ${ComponentName}
        FILES_MATCHING PATTERN
            "*.h")

        set(bodenConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake CACHE INTERNAL "")


    install(EXPORT airbnb_lottie_mac-export
        FILE
            airbnb_lottie_macTargets.cmake
        NAMESPACE
            Boden::
        DESTINATION
            ${bodenConfigPackageLocation}
        COMPONENT
          ${ComponentName})

elseif(BDN_PLATFORM_IOS)

    install(TARGETS airbnb_lottie_ios
        EXPORT airbnb_lottie_ios-export
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT
          ${ComponentName})

    install(DIRECTORY include/
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT
          ${ComponentName}
        FILES_MATCHING PATTERN
            "*.h")

        set(bodenConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake CACHE INTERNAL "")

    install(EXPORT airbnb_lottie_ios-export
        FILE
            airbnb_lottie_iosTargets.cmake
        NAMESPACE
            Boden::
        DESTINATION
            ${bodenConfigPackageLocation}
        COMPONENT
          ${ComponentName})

endif()


