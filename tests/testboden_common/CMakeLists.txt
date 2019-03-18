set( TESTBODEN_COMMON_SRCDIR "./src")

file(GLOB _TESTBODEN_COMMON_SOURCES
    ${TESTBODEN_COMMON_SRCDIR}/*.cpp
    ${TESTBODEN_COMMON_SRCDIR}/*.h
    ${TESTBODEN_COMMON_SRCDIR}/${BDN_TARGET}/*.cpp
    ${TESTBODEN_COMMON_SRCDIR}/${BDN_TARGET}/*.mm
    ${TESTBODEN_COMMON_SRCDIR}/${BDN_TARGET}/*.h
    ${TESTBODEN_COMMON_SRCDIR}/${BDN_TARGET}/*.hh)

add_library(testboden_common SHARED ${_TESTBODEN_COMMON_SOURCES})
target_link_libraries(testboden_common PUBLIC foundation testing ui)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_TESTBODEN_COMMON_SOURCES})

function(add_boden_test TARGET SOURCES PURE_CONSOLE)
    IF ( IOS )
        SET(APP_TYPE MACOSX_BUNDLE PARENT_SCOPE)
        SET(MACOSX_BUNDLE_BUNDLE_NAME ${TARGET} PARENT_SCOPE)
        SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.test.${TARGET}" PARENT_SCOPE)
    endif()
    
    if(PURE_CONSOLE)
        add_universal_executable(${TARGET} CONSOLE SOURCES ${SOURCES})
    else()
        add_universal_executable(${TARGET} SOURCES ${SOURCES})
    endif()
    
    target_link_libraries(${TARGET} PUBLIC testboden_common)
    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction()

add_clangformat(testboden_common)
