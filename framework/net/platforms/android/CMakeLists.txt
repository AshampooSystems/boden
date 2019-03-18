
add_platform_library(NAME android SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME ANDROID PARENT_LIBRARY net )


set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.android.volley:volley:1.1.1"
    CACHE STRING "The android dependencies" FORCE)





