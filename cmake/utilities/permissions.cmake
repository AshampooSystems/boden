macro(configure_app_permissions)
    set(options ALLOW_INTERNET ALLOW_HTTP ALLOW_CAMERA)

    cmake_parse_arguments(_permissions "${options}" "" "" ${ARGN} )

    if(BDN_PLATFORM_IOS OR BDN_PLATFORM_OSX)
        set(_transport_security "<key>NSAppTransportSecurity</key><dict>")

        # Allow HTTP
        string(APPEND _transport_security "<key>NSAllowsArbitraryLoadsInWebContent</key>")

        if(_permissions_ALLOW_HTTP)
            string(APPEND _transport_security "<true/>")
        else()
            string(APPEND _transport_security "<false/>")
        endif()

        string(APPEND _transport_security "</dict>")
        set(APPLE_NSAPP_TRANSPORT_SECURITY ${_transport_security})
    elseif(BDN_PLATFORM_ANDROID)
        set(ANDROID_PERMISSIONS "")

        if(_permissions_ALLOW_INTERNET)
            string(APPEND ANDROID_PERMISSIONS "<uses-permission android:name=\"android.permission.INTERNET\" />")
        endif()

        if(_permissions_ALLOW_CAMERA)
            string(APPEND ANDROID_PERMISSIONS "<uses-permission android:name=\"android.permission.CAMERA\" />")
        endif()
    endif()
endmacro()


