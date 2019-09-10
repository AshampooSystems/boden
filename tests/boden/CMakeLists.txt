file(GLOB property_tests ./properties/*.cpp)

add_universal_executable(testBoden TIDY SOURCES ../test_main.cpp
    testAttributedString.cpp
    testColor.cpp
    testContainerView.cpp
    testDispatchQueue.cpp
    testNotifier.cpp
    testValueWithFallback.cpp
    testProperties.cpp
    testPropertyStreaming.cpp
    testPropertyTransform.cpp
    testString.cpp
    testStyler.cpp
    testTimer.cpp
    testURI.cpp
    ${property_tests}
    TIDY)

target_link_libraries(testBoden PRIVATE gtest gtest_main Boden::All)

use_boden_template_info_plist(
    TARGET testBoden
    BUNDLE_NAME "Boden Unit tests"
    BUNDLE_ID "io.boden.testboden"
    )
set(ANDROID_APP_ID "io.boden.testboden")

#get_git_short_revision(GIT_REV)
#set_target_version(TARGET testBoden VERSION "0.1.0" SHORTVERSION "0.1" LONGVERSION "0.1.0.${GIT_REV}")

add_test( testBoden testBoden )
