file(GLOB _STACKDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND STACKDEMO_FILES ${_STACKDEMO_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME stackdemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.stackdemo")

ios_configure_asset(STACKDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)
ios_configure_asset(STACKDEMO_ASSETS ../DemoAssets/IOS_DemoIcons.xcassets)

configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

roger_add(
    stackdemo
    UIDEMO_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../DemoAssets/demo_resources.json
    "main")

add_universal_executable(stackdemo TIDY SOURCES ${STACKDEMO_FILES} ${STACKDEMO_ASSETS} ${UIDEMO_RESOURCES})

ios_configure_app_info(TARGET stackdemo
    LAUNCHSCREEN "LaunchScreen"
    APPICON "AppIcon"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

get_git_short_revision(GIT_REV)
set_target_version(TARGET stackdemo VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}")

use_boden_template_info_plist(stackdemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${STACKDEMO_FILES})

target_link_libraries(stackdemo PRIVATE Boden::All)

install(TARGETS stackdemo
    DESTINATION examples
    COMPONENT Examples)
