
set(_CMAKE_UTILITIES 
    utilities/linuxsysteminfo.cmake
    utilities/platform.cmake
    utilities/compiler.cmake
    utilities/warnings.cmake
    utilities/xcode.cmake
    utilities/ios.cmake
    utilities/android.cmake
    utilities/utilities.cmake
    utilities/install.cmake
    utilities/clangformat.cmake)

foreach(_utility ${_CMAKE_UTILITIES})
    include(${_utility})
endforeach()

set(_CMAKE_TOOLCHAINS
    toolchains/ios.xcode.toolchain.cmake
    toolchains/ios.make.toolchain.cmake
    toolchains/emscripten.toolchain.cmake)

set(_CMAKE_TEMPLATES
    templates/IOSInfo.plist.in
    templates/OSXInfo.plist.in
    templates/combined_header.h.in)

set(_CLANG_FORMAT
    ../.clang-format)

set(_CMAKE_SCRIPTS )

add_custom_target(boden_cmake SOURCES ${_CMAKE_UTILITIES} ${_CMAKE_TOOLCHAINS} ${_CMAKE_SCRIPTS} ${_CLANG_FORMAT} ${_CMAKE_TEMPLATES})

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_CMAKE_UTILITIES} ${_CMAKE_TOOLCHAINS} ${_CMAKE_SCRIPTS} ${_CMAKE_TEMPLATES})

