#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/testContainerViewCore.h>

#include <bdn/android/UiProvider.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/test/testAndroidViewCore.h>

using namespace bdn;

TEST_CASE("ContainerViewCore-android")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    pWindow->setContentView(pColumnView);

    SECTION("generic")
        bdn::test::testContainerViewCore(pWindow, pColumnView );        

    SECTION("android-ViewCore")
        bdn::android::test::testAndroidViewCore(pWindow, pColumnView, false);

    SECTION("android-ContainerViewCore")
    {
        P<bdn::android::ContainerViewCore> pCore = cast<bdn::android::ContainerViewCore>( pColumnView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        bdn::android::JView jv = pCore->getJView();
        REQUIRE( !jv.isNull_() );

        // there is nothing android-specific to test here.
        // The generic container view tests and the win32 view test have already tested
        // everything that the container view core does.        
    }
}




