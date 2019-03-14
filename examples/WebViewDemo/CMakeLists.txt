file(GLOB _WEBVIEWDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND WEBVIEWDEMO_FILES ${_WEBVIEWDEMO_SOURCES})


set(MACOSX_BUNDLE_BUNDLE_NAME webviewdemo)
set(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.webviewdemo")
set(MACOSX_BUNDLE_LAUNCH_STORYBOARD "LaunchScreen")

set(WEBVIEWDEMO_ASSETS
    ./ios-assets/Base.lproj/LaunchScreen.storyboard
    ./ios-assets/Assets.xcassets)


set(ANDROID_PERMISSIONS "<uses-permission android:name=\"android.permission.INTERNET\" />")

add_universal_executable(webviewdemo No ${WEBVIEWDEMO_FILES} ${WEBVIEWDEMO_ASSETS})

set_xcode_property(webviewdemo CODE_SIGN_STYLE Automatic)
set_xcode_property(webviewdemo ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)

if(BDN_PLATFORM_IOS)
    set_xcode_property(webviewdemo CODE_SIGN_IDENTITY "iPhone Developer")
    #set_xcode_property(webviewdemo DEVELOPMENT_TEAM <TeamID>)
endif()

set_target_properties( webviewdemo PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER ${MACOSX_BUNDLE_GUI_IDENTIFIER} )
set(MACOSX_NSAPP_TRANSPORT_SECURITY "<key>NSAppTransportSecurity</key><dict><key>NSAllowsArbitraryLoadsInWebContent</key><true/></dict>" )

use_boden_template_info_plist(webviewdemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${WEBVIEWDEMO_FILES})

target_link_libraries(webviewdemo PRIVATE ui foundation yogalayout webview nlohmann_json::nlohmann_json)

install(TARGETS webviewdemo
    DESTINATION examples
    COMPONENT Examples)
