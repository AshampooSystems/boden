add_platform_library(NAME ios SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME IOS PARENT_LIBRARY lottieview )

target_link_libraries(lottieview_ios PRIVATE airBnB::Lottie)
