file(GLOB _KEEWEB_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND KEEWEB_FILES ${_KEEWEB_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME keeweb)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.keeweb")

ios_configure_asset(KEEWEB_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)


configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP ALLOW_CAMERA)

set(MACOSX_BUNDLE_BUNDLE_NAME KeeWeb)
set(ANDROID_THEME "@style/Theme.AppCompat.NoActionBar")

roger_add(
    keeweb
    KEEWEB_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/keeweb_assets.json)

roger_add(
    keeweb
    KEEWEB_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../DemoIcon/demo_icon.json)

set(ANDROID_ICON "android:icon=\"@mipmap/ic_launcher_png\"")

add_universal_executable(keeweb TIDY SOURCES ${KEEWEB_FILES} ${KEEWEB_ASSETS} ${KEEWEB_RESOURCES})

ios_configure_app_info(TARGET keeweb
    LAUNCHSCREEN "LaunchScreen"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

set_xcode_appicon_asset(keeweb "ApplicationIcon")


get_git_short_revision(GIT_REV)
set_target_version(TARGET keeweb VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}")

use_boden_template_info_plist(keeweb)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${KEEWEB_FILES})

target_link_libraries(keeweb PRIVATE Boden::All)

install(TARGETS keeweb
    DESTINATION examples
    COMPONENT Examples)
