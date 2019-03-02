file(GLOB _UIDEMO_SOURCES ./src/*.cpp ./src/*.h)

list(APPEND UIDEMO_FILES ${_UIDEMO_SOURCES})


IF ( BDN_PLATFORM_IOS )
	SET(MACOSX_BUNDLE_BUNDLE_NAME uidemo)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.uidemo")
	SET(MACOSX_BUNDLE_LAUNCH_STORYBOARD "LaunchScreen")

	set(UIDEMO_ASSETS 
		./ios-assets/Base.lproj/LaunchScreen.storyboard
		./ios-assets/Assets.xcassets)


endif()


add_universal_executable(uidemo No ${UIDEMO_FILES} ${UIDEMO_ASSETS})

set_xcode_property(uidemo CODE_SIGN_STYLE Automatic)
set_xcode_property(uidemo ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
set_xcode_property(uidemo CODE_SIGN_IDENTITY "iPhone Developer")
#set_xcode_property(uidemo DEVELOPMENT_TEAM <TeamID>)

use_boden_template_info_plist(uidemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${UIDEMO_FILES})

target_link_libraries(uidemo PRIVATE ui foundation yogalayout nlohmann_json::nlohmann_json)

install(TARGETS uidemo
    DESTINATION examples
    COMPONENT Examples)
