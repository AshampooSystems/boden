add_platform_library(ios ${CMAKE_CURRENT_LIST_DIR} IOS lottieview )

target_link_libraries(lottieview_ios PRIVATE airBnB::Lottie)
