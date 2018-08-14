
macro(disable_deprecated_register_warning TARGET)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${TARGET} PRIVATE "-Wno-deprecated-register")
    endif()
endmacro()

macro(disable_deprecated_declarations_warning TARGET SCOPE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET} PRIVATE "-Wno-deprecated-declarations")
    endif()
endmacro()
