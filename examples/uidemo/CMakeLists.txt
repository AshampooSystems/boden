file(GLOB _UIDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND UIDEMO_FILES ${_UIDEMO_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME uidemo)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.uidemo")

ios_configure_asset(UIDEMO_ASSETS ../IOS_DemoIcons.xcassets)
ios_configure_asset(UIDEMO_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)

configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

add_universal_executable(uidemo TIDY SOURCES ${UIDEMO_FILES} ${UIDEMO_ASSETS})

ios_setup_code_signing(uidemo)
ios_set_appicon_asset_file(uidemo "AppIcon")
ios_set_launchboard_file("LaunchScreen")

use_boden_template_info_plist(uidemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${UIDEMO_FILES})

target_link_libraries(uidemo PRIVATE Boden::All)

install(TARGETS uidemo
    DESTINATION examples
    COMPONENT Examples)
