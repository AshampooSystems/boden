set(_sources 
	./source/AppController.h
	./source/AppController.cpp
	./source/MainViewController.h
	./source/MainViewController.cpp
	./source/main.cpp
)

ios_configure_asset(KEYBOARDTEST_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)

roger_add(
    KeyboardTest
    BODENDEMO_RESOURCES
    ${PROJECT_SOURCE_DIR}/examples/DemoIcon/demo_icon.json)

set(ANDROID_ICON "android:icon=\"@mipmap/ic_launcher_png\"")

add_universal_executable(KeyboardTest SOURCES ${_sources} ${KEYBOARDTEST_ASSETS} ${BODENDEMO_RESOURCES})

ios_configure_app_info(TARGET KeyboardTest
    LAUNCHSCREEN "LaunchScreen"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

set_xcode_appicon_asset(KeyboardTest "ApplicationIcon")

get_git_short_revision(GIT_REV)
set_target_version(TARGET KeyboardTest VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}" ANDROID_VERSION_ID 1)

use_boden_template_info_plist(
    TARGET KeyboardTest
    BUNDLE_NAME "Keyboard Test"
    BUNDLE_ID "io.boden.keyboardtest"
    )
set(ANDROID_APP_ID "io.boden.keyboardtest")

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_sources})

target_link_libraries(KeyboardTest ui yoga)
