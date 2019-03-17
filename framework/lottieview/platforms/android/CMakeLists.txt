add_platform_library(android ${CMAKE_CURRENT_LIST_DIR} ANDROID lottieview )

set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.airbnb.android:lottie:2.8.0"
    CACHE STRING "The android dependencies" FORCE)
