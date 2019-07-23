macro(use_boden_template_info_plist TARGETNAME)
    if(BDN_PLATFORM_IOS)
        get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)
        set_target_properties(${TARGETNAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${BODEN_CMAKE_SOURCE_DIR}/templates/IOSInfo.plist.in)
    elseif(BDN_PLATFORM_OSX)
        get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)
        set_target_properties(${TARGETNAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${BODEN_CMAKE_SOURCE_DIR}/templates/OSXInfo.plist.in)
    endif()
endmacro()

macro(ios_setup_code_signing TARGETNAME)
    if(BDN_PLATFORM_IOS)
        set_xcode_property(${TARGETNAME} CODE_SIGN_STYLE Automatic)
        set_xcode_property(${TARGETNAME} CODE_SIGN_IDENTITY "iPhone Developer")

        if (DEFINED ENV{BODEN_TEAM_ID})
            set_xcode_property(${TARGETNAME} DEVELOPMENT_TEAM $ENV{BODEN_TEAM_ID})
        endif()
    endif()
endmacro()

macro(ios_targeted_devices )
    if(BDN_PLATFORM_IOS)
        set(options IPHONE IPAD)
        set(oneValueArgs TARGET)
        set(multiValueArgs )
        cmake_parse_arguments(_TARGET "${options}" "${oneValueArgs}"
            "${multiValueArgs}" ${ARGN} )

        if(_TARGET_IPHONE AND _TARGET_IPAD)
            set_xcode_property (${_TARGET_TARGET} TARGETED_DEVICE_FAMILY "1,2")
        elseif(_TARGET_IPHONE)
            set_xcode_property (${_TARGET_TARGET} TARGETED_DEVICE_FAMILY "1")
        elseif(_TARGET_IPAD)
            set_xcode_property (${_TARGET_TARGET} TARGETED_DEVICE_FAMILY "2")
        endif()
    endif()
endmacro()

macro(ios_build_orientation_entries OUT )
    set(options ALL PORTRAIT LANDSCAPE_LEFT LANDSCAPE_RIGHT UPSIDEDOWN)
    set(oneValueArgs )
    set(multiValueArgs )
    cmake_parse_arguments(_ARGUMENTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(${OUT} "    <array>\n")

    if(_ARGUMENTS_ALL OR _ARGUMENTS_PORTRAIT)
        set(${OUT} "${${OUT}}        <string>UIInterfaceOrientationPortrait</string>\n")
    endif()
    if(_ARGUMENTS_ALL OR _ARGUMENTS_LANDSCAPE_LEFT)
        set(${OUT} "${${OUT}}        <string>UIInterfaceOrientationLandscapeLeft</string>\n")
    endif()
    if(_ARGUMENTS_ALL OR _ARGUMENTS_LANDSCAPE_RIGHT)
        set(${OUT} "${${OUT}}        <string>UIInterfaceOrientationLandscapeRight</string>\n")
    endif()
    if(_ARGUMENTS_ALL OR _ARGUMENTS_UPSIDEDOWN)
        set(${OUT} "${${OUT}}        <string>UIInterfaceOrientationPortraitUpsideDown</string>\n")
    endif()

    set(${OUT} "${${OUT}}    </array>\n")
endmacro()

macro(ios_supported_orientations)
    if(BDN_PLATFORM_IOS)
        set(options)
        set(oneValueArgs)
        set(multiValueArgs IPHONE IPAD)
        cmake_parse_arguments(_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

        set(result "")
        if(_TARGET_IPHONE)
            ios_build_orientation_entries(_iphone_entries ${_TARGET_IPHONE})
            set(result "${result}    <key>UISupportedInterfaceOrientations</key>\n${_iphone_entries}")
        endif()
        if(_TARGET_IPAD)
            ios_build_orientation_entries(_ipad_entries ${_TARGET_IPAD})
            set(result "${result}    <key>UISupportedInterfaceOrientations~ipad</key>\n${_ipad_entries}")
        endif()

        set(IOS_SUPPORTED_INTERFACE_ORIENTATIONS ${result})
    endif()
endmacro()

macro(ios_set_launchboard_file LAUNCHBOARD_NAME)
    SET(MACOSX_BUNDLE_LAUNCH_STORYBOARD ${LAUNCHBOARD_NAME})
endmacro()

macro(ios_configure_asset ASSET_LIST ASSET_FILE)
    if(BDN_PLATFORM_IOS)
        set_source_files_properties(${ASSET_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        list(APPEND ${ASSET_LIST} ${ASSET_FILE})
    endif()
endmacro()

macro(ios_configure_app_info)
    if(BDN_PLATFORM_IOS)
        set(options )
        set(oneValueArgs TARGET LAUNCHSCREEN)
        set(multiValueArgs TARGETED_DEVICES IPHONE_ORIENTATIONS IPAD_ORIENTATIONS)
        cmake_parse_arguments(_ARGUMENTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

        set_xcode_property(${_ARGUMENTS_TARGET} INSTALL_PATH "$(LOCAL_APPS_DIR)")

        ios_setup_code_signing(${_ARGUMENTS_TARGET})
        ios_set_launchboard_file(${_ARGUMENTS_LAUNCHSCREEN})

        if(_ARGUMENTS_TARGETED_DEVICES)
            ios_targeted_devices(TARGET ${_ARGUMENTS_TARGET} ${_ARGUMENTS_TARGETED_DEVICES})
        endif()

        ios_supported_orientations(IPHONE ${_ARGUMENTS_IPHONE_ORIENTATIONS} IPAD ${_ARGUMENTS_IPAD_ORIENTATIONS})
    endif()
endmacro()

macro(ios_fix_lib_build_folder TARGET)
    if(BDN_PLATFORM_IOS AND XCODE)

        set_target_properties(${TARGET} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_DEBUG "$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/")
        set_target_properties(${TARGET} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE "$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/")
        set_target_properties(${TARGET} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/")
        set_target_properties(${TARGET} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/")

    endif()
endmacro()
