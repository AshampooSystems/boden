file(GLOB _NAVIGATIONVIEWDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND NAVIGATIONVIEWDEMO_FILES ${_NAVIGATIONVIEWDEMO_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME navigationviewdemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.navigationviewdemo")

ios_configure_asset(NAVIGATIONVIEWDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)
ios_configure_asset(NAVIGATIONVIEWDEMO_ASSETS ../DemoAssets/IOS_DemoIcons.xcassets)

configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

roger_add(
    navigationviewdemo
    UIDEMO_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../DemoAssets/demo_resources.json
    "main")

add_universal_executable(navigationviewdemo TIDY SOURCES ${NAVIGATIONVIEWDEMO_FILES} ${NAVIGATIONVIEWDEMO_ASSETS} ${UIDEMO_RESOURCES})

ios_configure_app_info(TARGET navigationviewdemo
    LAUNCHSCREEN "LaunchScreen"
    APPICON "AppIcon"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

get_git_short_revision(GIT_REV)
set_target_version(TARGET navigationviewdemo VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}")

use_boden_template_info_plist(navigationviewdemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${NAVIGATIONVIEWDEMO_FILES})

target_link_libraries(navigationviewdemo PRIVATE Boden::All)

install(TARGETS navigationviewdemo
    DESTINATION examples
    COMPONENT Examples)
