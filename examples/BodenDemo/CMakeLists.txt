file(GLOB _BODENDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND BODENDEMO_FILES ${_BODENDEMO_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME bodendemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.bodendemo")

ios_configure_asset(BODENDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)
ios_configure_asset(BODENDEMO_ASSETS ${PROJECT_SOURCE_DIR}/examples/DemoIcon/black.png)

configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

roger_add(
    bodendemo
    BODENDEMO_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/resources.json)

roger_add(
    bodendemo
    BODENDEMO_RESOURCES
    ${PROJECT_SOURCE_DIR}/examples/DemoIcon/demo_icon.json)

set(ANDROID_ICON "android:icon=\"@mipmap/ic_launcher_png\"")

add_universal_executable(bodendemo TIDY SOURCES ${BODENDEMO_FILES} ${BODENDEMO_ASSETS} ${BODENDEMO_RESOURCES})

ios_configure_app_info(TARGET bodendemo
    LAUNCHSCREEN "LaunchScreen"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

set_xcode_appicon_asset(bodendemo "ApplicationIcon")

get_git_short_revision(GIT_REV)
set_target_version(TARGET bodendemo VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}" ANDROID_VERSION_ID 2)

use_boden_template_info_plist(bodendemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${BODENDEMO_FILES})

target_link_libraries(bodendemo PRIVATE Boden::All)

install(TARGETS bodendemo
    DESTINATION examples
    COMPONENT Examples)
