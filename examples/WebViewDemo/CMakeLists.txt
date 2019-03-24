file(GLOB _WEBVIEWDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND WEBVIEWDEMO_FILES ${_WEBVIEWDEMO_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME webviewdemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.webviewdemo")

ios_configure_asset(WEBVIEWDEMO_ASSETS ../DemoAssets/IOS_DemoIcons.xcassets)
ios_configure_asset(WEBVIEWDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)


configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

add_universal_executable(webviewdemo TIDY SOURCES ${WEBVIEWDEMO_FILES} ${WEBVIEWDEMO_ASSETS})

ios_configure_app_info(TARGET webviewdemo
    LAUNCHSCREEN "LaunchScreen"
    APPICON "AppIcon"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

get_git_short_revision(GIT_REV)
set_target_version(TARGET webviewdemo VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}")

use_boden_template_info_plist(webviewdemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${WEBVIEWDEMO_FILES})

target_link_libraries(webviewdemo PRIVATE Boden::All)

install(TARGETS webviewdemo
    DESTINATION examples
    COMPONENT Examples)
