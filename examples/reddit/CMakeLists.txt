file(GLOB _REDDIT_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND REDDIT_FILES ${_REDDIT_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME reddit)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.reddit")

ios_configure_asset(REDDIT_ASSETS ../IOS_DemoIcons.xcassets)
ios_configure_asset(REDDIT_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)


configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

add_universal_executable(reddit TIDY SOURCES ${REDDIT_FILES} ${REDDIT_ASSETS})

ios_setup_code_signing(reddit)
ios_set_appicon_asset_file(reddit "AppIcon")
ios_set_launchboard_file("LaunchScreen")

use_boden_template_info_plist(reddit)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${REDDIT_FILES})

target_link_libraries(reddit PRIVATE Boden::All)

install(TARGETS reddit
    DESTINATION examples
    COMPONENT Examples)
