add_platform_library(NAME android SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME ANDROID PARENT_LIBRARY lottieview )

set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.airbnb.android:lottie:2.8.0"
    CACHE STRING "The android dependencies" FORCE)
