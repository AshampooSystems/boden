set(_sources 
    ./source/AppController.h
    ./source/AppController.cpp
    ./source/MainViewController.h
    ./source/MainViewController.cpp
    ./source/main.cpp
    ./source/TodoListData.cpp
    ./source/TodoListData.h
    ./source/TodoListItem.cpp
    ./source/TodoListItem.h
)

set(MACOSX_BUNDLE_BUNDLE_NAME TodoMVC)
set(MACOSX_BUNDLE_GUI_IDENTIFIER "io.mycompany.TodoMVC")

ios_configure_asset(_assets ./ios-assets/Base.lproj/LaunchScreen.storyboard)
ios_configure_asset(_assets DemoIcon/black.png)

roger_add(
    TodoMVC
    _assets
    ${CMAKE_CURRENT_LIST_DIR}/DemoIcon/demo_icon.json)

set(ANDROID_ICON "android:icon=\"@mipmap/ic_launcher_png\"")

add_universal_executable(TodoMVC SOURCES ${_sources} ${_assets})

ios_configure_app_info(TARGET TodoMVC
    LAUNCHSCREEN "LaunchScreen"
    TARGETED_DEVICES IPHONE IPAD
    IPHONE_ORIENTATIONS ALL
    IPAD_ORIENTATIONS ALL)

set_xcode_appicon_asset(TodoMVC "ApplicationIcon")
use_boden_template_info_plist(TodoMVC)

get_git_short_revision(GIT_REV)
set_target_version(TARGET TodoMVC VERSION "0.2.0" SHORTVERSION "0.2" LONGVERSION "0.2.0.${GIT_REV}" ANDROID_VERSION_ID 1)

target_link_libraries(TodoMVC ui yoga)
