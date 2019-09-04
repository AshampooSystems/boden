include(CheckCXXSourceCompiles)

add_platform_library(NAME android SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME Android PARENT_LIBRARY foundation)

find_library(ANDROID_LOG_LIB log)
mark_as_advanced(ANDROID_LOG_LIB)
target_link_libraries(foundation_android PUBLIC ${ANDROID_LOG_LIB})

set(BAUER_ANDROID_DEPENDENCIES
    ${BAUER_ANDROID_DEPENDENCIES}
    "com.android.support:appcompat-v7:${ANDROID_NATIVE_API_LEVEL}.+"
    CACHE STRING "The android dependencies" FORCE)

if(NOT BDN_NEVER_INCLUDE_STD_FILESYSTEM_POLYFILL)
    set(CMAKE_REQUIRED_FLAGS "-std=c++17")
    CHECK_CXX_SOURCE_COMPILES("#include <filesystem>\nint main() { std::filesystem::space(\"/\"); }" HAS_STD_FILESYSTEM)

    if(NOT HAS_STD_FILESYSTEM)
        message(STATUS "Including custom std::filesystem implementation")

        target_sources(foundation_android PRIVATE
            filesystem/operations.cpp
            filesystem/directory_iterator.cpp
            filesystem/filesystem_common.h
            filesystem/int128_builtins.cpp)
    endif()
endif()
