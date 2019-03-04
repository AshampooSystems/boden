add_platform_library(ios ${CMAKE_CURRENT_LIST_DIR} iOS foundation)

target_link_libraries(foundation_ios PUBLIC "-framework UIKit" "-framework Foundation" "-framework CoreGraphics" "-framework WebKit")
target_link_libraries(foundation_ios INTERFACE foundation_fk)

set_xcode_property(foundation_ios CODE_SIGN_IDENTITY "")
set_xcode_property(foundation_ios CODE_SIGNING_REQUIRED "NO")

#set_xcode_property(boden CODE_SIGN_IDENTITY "iPhone Developer")
#set_xcode_property(boden CODE_SIGN_STYLE "Automatic")
#set_xcode_property(boden PROVISIONING_PROFILE "Automatic")
#set_xcode_property(boden DEVELOPMENT_TEAM "X")
