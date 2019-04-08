SET(MACOSX_BUNDLE_BUNDLE_NAME testFoundation)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.testFoundation")

file(GLOB property_tests ./properties/*.cpp)


add_universal_executable(testFoundation SOURCES ../test_main.cpp
    testNotifier.cpp
    testProperties.cpp
    testPropertyStreaming.cpp
    testPropertyTransform.cpp
    testOfferedValue.cpp
    testTimer.cpp
    testURI.cpp
    ${property_tests}
    TIDY)

target_link_libraries(testFoundation PRIVATE gtest gtest_main Boden::All)

add_test( testFoundation testFoundation )
