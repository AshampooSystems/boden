add_platform_library(NAME android SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME ANDROID PARENT_LIBRARY ui )

set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.android.support.constraint:constraint-layout:1.0.2"
    "com.android.support:recyclerview-v7:28.0.0" 
    "com.android.support:recyclerview-selection:28.+"
    CACHE STRING "The android dependencies" FORCE)






