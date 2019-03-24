file(GLOB _UIDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND UIDEMO_FILES ${_UIDEMO_SOURCES} ${UIDEMO_RESOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME uidemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.uidemo")

ios_configure_asset(UIDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)
ios_configure_asset(UIDEMO_ASSETS ../DemoAssets/IOS_DemoIcons.xcassets)

configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

roger_add(
    uidemo
    UIDEMO_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../DemoAssets/demo_resources.json
    "main")

add_universal_executable(uidemo TIDY SOURCES ${UIDEMO_FILES} ${UIDEMO_ASSETS} ${UIDEMO_RESOURCES})

ios_configure_app_info(TARGET uidemo
    LAUNCHSCREEN "LaunchScreen"
    APPICON "AppIcon"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

get_git_short_revision(GIT_REV)
set_target_version(TARGET uidemo VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}")

use_boden_template_info_plist(uidemo)

target_link_libraries(uidemo PRIVATE Boden::All)

install(TARGETS uidemo
    DESTINATION examples
    COMPONENT Examples)
