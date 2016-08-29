#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/android/UiProvider.h>
#include <bdn/android/WindowCore.h>

#include "testAndroidViewCore.h"

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

    SECTION("View destroyed when object destroyed")
    {
        // there may be pending sizing info updates for the window, which keep it alive.
        // Ensure that those are done first.

        // wrap pWindow in a struct so that we can destroy all references
        // in the continuation.
        struct CaptureData : public Base
        {
            P<Window> pWindow;
        };
        P<CaptureData> pData = newObj<CaptureData>();
        pData->pWindow = pWindow;
        pWindow = nullptr;

        CONTINUE_SECTION_ASYNC(pData)
        {
            P<bdn::android::WindowCore> pCore = cast<bdn::android::WindowCore>( pData->pWindow->getViewCore() );
            REQUIRE( pCore!=nullptr );

            // we cannot predict when the garbage collector will actually destroy
            // the view object. So we cannot test for actual object destruction.
            // Instead we verify that the view is removed from its parent.

            bdn::android::JView jv = pCore->getJView();
            REQUIRE( !jv.isNull_() );

            // sanity check
            REQUIRE( !jv.getParent().isNull_() );

            SECTION("core not kept alive")
            {
                pCore = nullptr;
            }

            SECTION("core kept alive")
            {
                // do nothing
            }

            pData->pWindow = nullptr;

            // we cannot predu

            // view object should have been removed from its parent
            REQUIRE( jv.getParent().isNull_() );
        };
    }
}




