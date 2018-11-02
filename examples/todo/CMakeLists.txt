file(GLOB _TODO_SOURCES 
	./src/*.cpp
	./src/*.h)

list(APPEND TODO_FILES ${_TODO_SOURCES})


IF ( BDN_PLATFORM_IOS )
	SET(MACOSX_BUNDLE_BUNDLE_NAME todo)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.todo")
	SET(MACOSX_BUNDLE_LAUNCH_STORYBOARD "LaunchScreen")

	set(TODO_ASSETS 
		./ios-assets/Base.lproj/LaunchScreen.storyboard
		./ios-assets/Assets.xcassets)
endif()


add_universal_executable(todo No ${TODO_FILES} ${TODO_ASSETS})

set_xcode_property(todo CODE_SIGN_STYLE Automatic)
set_xcode_property(todo ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
set_xcode_property(todo CODE_SIGN_IDENTITY "iPhone Developer")
#set_xcode_property(todo DEVELOPMENT_TEAM <YourTeamId>)

use_boden_template_info_plist(todo)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${TODO_FILES})

target_link_libraries(todo PRIVATE ui foundation)

install(TARGETS todo
    DESTINATION examples
    COMPONENT Examples)
