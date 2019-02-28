add_platform_library(android ${CMAKE_CURRENT_LIST_DIR} ANDROID ui )

set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.android.support.constraint:constraint-layout:1.0.2"
    CACHE STRING "The android dependencies" FORCE)
