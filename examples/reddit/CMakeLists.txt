file(GLOB _REDDIT_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND REDDIT_FILES ${_REDDIT_SOURCES})

SET(MACOSX_BUNDLE_BUNDLE_NAME reddit)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.reddit")

ios_configure_asset(REDDIT_ASSETS ../DemoAssets/IOS_DemoIcons.xcassets)
ios_configure_asset(REDDIT_ASSETS ./ios-assets/Base.lproj/LaunchScreen.storyboard)


configure_app_permissions(ALLOW_INTERNET ALLOW_HTTP)

add_universal_executable(reddit TIDY SOURCES ${REDDIT_FILES} ${REDDIT_ASSETS})

ios_configure_app_info(TARGET reddit
    LAUNCHSCREEN "LaunchScreen"
    APPICON "AppIcon"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

get_git_short_revision(GIT_REV)
set_target_version(TARGET reddit VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}")


use_boden_template_info_plist(reddit)

target_link_libraries(reddit PRIVATE Boden::All)

install(TARGETS reddit
    DESTINATION examples
    COMPONENT Examples)
