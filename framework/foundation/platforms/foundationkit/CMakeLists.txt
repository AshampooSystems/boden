include(CheckIncludeFileCXX)

add_platform_library(NAME fk SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME Base PARENT_LIBRARY foundation)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT BDN_NEVER_INCLUDE_STD_FILESYSTEM_POLYFILL)
    CHECK_INCLUDE_FILE_CXX("filesystem" HAS_STD_FILESYSTEM)

    if(NOT HAS_STD_FILESYSTEM)
        message(STATUS "Including custom std::filesystem implementation")

        target_sources(foundation_fk PRIVATE
            ./filesystem/include/filesystem
            ./filesystem/src/operations.cpp
            ./filesystem/src/directory_iterator.cpp
            ./filesystem/src/int128_builtins.cpp
            ./filesystem/src/filesystem_common.h)

        target_include_directories(foundation_fk
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/filesystem/include>
            )
    endif()
endif()
