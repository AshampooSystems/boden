add_platform_library(android ${CMAKE_CURRENT_LIST_DIR} Android foundation)

find_library(ANDROID_LOG_LIB log)
mark_as_advanced(ANDROID_LOG_LIB)
target_link_libraries(foundation_android PUBLIC ${ANDROID_LOG_LIB})

set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.android.support:appcompat-v7:${ANDROID_NATIVE_API_LEVEL}.+"
    CACHE STRING "The android dependencies" FORCE)


#message(FATAL_ERROR "ANDROID LOG: ${ANDROID_LOG_LIB}" )
