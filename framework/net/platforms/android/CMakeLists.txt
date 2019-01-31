
add_platform_library(android ${CMAKE_CURRENT_LIST_DIR} ANDROID net )


set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.android.volley:volley:1.1.1"
    CACHE STRING "The android dependencies" FORCE)





