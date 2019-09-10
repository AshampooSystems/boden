cmake_minimum_required(VERSION 3.6)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

set(BDN_BUILD_TESTS OFF CACHE BOOL "Don't include boden tests")
set(BDN_BUILD_EXAMPLES OFF CACHE BOOL "Don't include boden examples")
set(BDN_WARNINGS_AS_ERRORS OFF CACHE BOOL "Don't treat warnings as errors")

project({project_name})

add_subdirectory({boden_relative_path} boden)

set(_sources 
	./source/AppController.h
	./source/AppController.cpp
	./source/MainViewController.h
	./source/MainViewController.cpp
	./source/main.cpp
)

ios_configure_asset(_assets ./ios-assets/Base.lproj/LaunchScreen.storyboard)
ios_configure_asset(_assets DemoIcon/black.png)

roger_add(
    {project_name}
    _assets
    ${CMAKE_CURRENT_LIST_DIR}/DemoIcon/demo_icon.json)

set(ANDROID_ICON "android:icon=\"@mipmap/ic_launcher_png\"")

add_universal_executable({project_name} SOURCES ${_sources} ${_assets})

ios_configure_app_info(TARGET {project_name}
    LAUNCHSCREEN "LaunchScreen"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

use_boden_template_info_plist(
    TARGET {project_name}
    BUNDLE_NAME "{project_name}"
    BUNDLE_ID "io.mycompany.{project_name}"
    )
set(ANDROID_APP_ID "io.mycompany.{project_name}")

set_xcode_appicon_asset({project_name} "ApplicationIcon")
use_boden_template_info_plist({project_name})

set_target_version(TARGET {project_name} VERSION "1.0.0" SHORTVERSION "1.0" LONGVERSION "1.0.0.0" ANDROID_VERSION_ID 1)

target_link_libraries({project_name} Boden::All)
