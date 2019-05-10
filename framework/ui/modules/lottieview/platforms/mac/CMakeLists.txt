add_platform_library(NAME mac SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME MacOSX PARENT_LIBRARY lottieview )

target_link_libraries(lottieview_mac PRIVATE airBnB::Lottie)
