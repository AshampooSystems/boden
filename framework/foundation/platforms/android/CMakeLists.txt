add_platform_library(android ${CMAKE_CURRENT_LIST_DIR} iOS foundation)

find_library(ANDROID_LOG_LIB log)
mark_as_advanced(ANDROID_LOG_LIB)
target_link_libraries(foundation_android PUBLIC ${ANDROID_LOG_LIB})

#message(FATAL_ERROR "ANDROID LOG: ${ANDROID_LOG_LIB}" )
