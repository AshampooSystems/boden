file(GLOB _UIDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND UIDEMO_FILES ${_UIDEMO_SOURCES})


SET(MACOSX_BUNDLE_BUNDLE_NAME uidemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.uidemo")
SET(MACOSX_BUNDLE_LAUNCH_STORYBOARD "LaunchScreen")

set(UIDEMO_ASSETS
    ./ios-assets/Base.lproj/LaunchScreen.storyboard
    ./ios-assets/Assets.xcassets)

set(ANDROID_PERMISSIONS "<uses-permission android:name=\"android.permission.INTERNET\" />")

add_universal_executable(uidemo No ${UIDEMO_FILES} ${UIDEMO_ASSETS})

set(MACOSX_NSAPP_TRANSPORT_SECURITY "<key>NSAppTransportSecurity</key><dict><key>NSAllowsArbitraryLoadsInWebContent</key><true/></dict>" )

if(BDN_PLATFORM_IOS)
    set_xcode_property(uidemo CODE_SIGN_STYLE Automatic)
    set_xcode_property(uidemo ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
    set_xcode_property(uidemo CODE_SIGN_IDENTITY "iPhone Developer")
    #set_xcode_property(uidemo DEVELOPMENT_TEAM <TeamID>)
endif()

use_boden_template_info_plist(uidemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${UIDEMO_FILES})

target_link_libraries(uidemo PRIVATE ui foundation yogalayout lottieview nlohmann_json::nlohmann_json)

install(TARGETS uidemo
    DESTINATION examples
    COMPONENT Examples)
