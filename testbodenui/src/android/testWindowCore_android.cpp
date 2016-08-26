#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/android/UiProvider.h>
#include <bdn/android/WindowCore.h>
#include <bdn/android/test/testAndroidViewCore.h>

using namespace bdn;

TEST_CASE("WindowCore-android")
{
    P<Window> pWindow = newObj<Window>( &bdn::android::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("android-view")
        bdn::android::test::testAndroidViewCore(pWindow, pWindow, false);

    SECTION("android-window")
    {
        P<bdn::android::WindowCore> pCore = cast<bdn::android::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        bdn::android::JView jv = pCore->getJView();
        REQUIRE( !jv.isNull_() );

        SECTION("title")
        {
            pWindow->title() = "hello world";

            // window title changes are ignored on android, so there is nothing to test here.
        }                
    }
}




