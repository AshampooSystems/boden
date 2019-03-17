file(GLOB _STACKDEMO_SOURCES ./src/*.cpp)

list(APPEND STACKDEMO_FILES ${_STACKDEMO_SOURCES})

IF ( BDN_PLATFORM_IOS )
	SET(MACOSX_BUNDLE_BUNDLE_NAME stackdemo)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.stackdemo")
	SET(MACOSX_BUNDLE_LAUNCH_STORYBOARD "LaunchScreen")

	set(STACKDEMO_ASSETS 
		./ios-assets/Base.lproj/LaunchScreen.storyboard
		./ios-assets/Assets.xcassets)
endif()

add_universal_executable(stackdemo No ${STACKDEMO_FILES} ${STACKDEMO_ASSETS})

set_xcode_property(stackdemo CODE_SIGN_STYLE Automatic)
set_xcode_property(stackdemo ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
set_xcode_property(stackdemo CODE_SIGN_IDENTITY "iPhone Developer")
#set_xcode_property(stackdemo DEVELOPMENT_TEAM <TeamID>)

use_boden_template_info_plist(stackdemo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${STACKDEMO_FILES})

target_link_libraries(stackdemo PRIVATE ui foundation yogalayout lottieview nlohmann_json::nlohmann_json)

install(TARGETS stackdemo
    DESTINATION examples
    COMPONENT Examples)
