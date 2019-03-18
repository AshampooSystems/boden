file(GLOB _WEBVIEWDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND WEBVIEWDEMO_FILES ${_WEBVIEWDEMO_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME webviewdemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.webviewdemo")

ios_configure_asset(WEBVIEWDEMO_ASSETS ../IOS_DemoIcons.xcassets)
ios_configure_asset(WEBVIEWDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)


configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

add_universal_executable(webviewdemo TIDY SOURCES ${WEBVIEWDEMO_FILES} ${WEBVIEWDEMO_ASSETS})

ios_setup_code_signing(webviewdemo)
ios_set_appicon_asset_file(webviewdemo "AppIcon")
ios_set_launchboard_file("LaunchScreen")

use_boden_template_info_plist(webviewdemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${WEBVIEWDEMO_FILES})

target_link_libraries(webviewdemo PRIVATE Boden::All)

install(TARGETS webviewdemo
    DESTINATION examples
    COMPONENT Examples)
