add_platform_library(mac ${CMAKE_CURRENT_LIST_DIR} MacOSX lottieview )

target_link_libraries(lottieview_mac PRIVATE airBnB::Lottie)
