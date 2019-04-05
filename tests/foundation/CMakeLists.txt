SET(MACOSX_BUNDLE_BUNDLE_NAME testFoundation)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.testFoundation")

add_universal_executable(testFoundation SOURCES ../test_main.cpp
    testNotifier.cpp
    testProperties.cpp
    testOfferedValue.cpp
    testURI.cpp
    TIDY)

target_link_libraries(testFoundation PRIVATE gtest gtest_main Boden::All)

add_test( testFoundation testFoundation )
