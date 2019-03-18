file(GLOB _STACKDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND STACKDEMO_FILES ${_STACKDEMO_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME stackdemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.stackdemo")

ios_configure_asset(STACKDEMO_ASSETS ../IOS_DemoIcons.xcassets)
ios_configure_asset(STACKDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)

configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

add_universal_executable(stackdemo TIDY SOURCES ${STACKDEMO_FILES} ${STACKDEMO_ASSETS})

ios_setup_code_signing(stackdemo)
ios_set_appicon_asset_file(stackdemo "AppIcon")
ios_set_launchboard_file("LaunchScreen")

use_boden_template_info_plist(stackdemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${STACKDEMO_FILES})

target_link_libraries(stackdemo PRIVATE Boden::All)

install(TARGETS stackdemo
    DESTINATION examples
    COMPONENT Examples)
