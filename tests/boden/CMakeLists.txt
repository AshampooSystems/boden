SET(MACOSX_BUNDLE_BUNDLE_NAME testBoden)
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.testBoden")

file(GLOB property_tests ./properties/*.cpp)


add_universal_executable(testBoden TIDY SOURCES ../test_main.cpp
    testNotifier.cpp
    testProperties.cpp
    testPropertyStreaming.cpp
    testPropertyTransform.cpp
    testOfferedValue.cpp
    testDispatchQueue.cpp
    testTimer.cpp
    testString.cpp
    testURI.cpp
    testStyler.cpp
    testColor.cpp
    ${property_tests}
    TIDY)

target_link_libraries(testBoden PRIVATE gtest gtest_main Boden::All)

add_test( testBoden testBoden )
