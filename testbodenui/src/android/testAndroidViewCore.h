#ifndef BDN_ANDROID_TEST_testAndroidViewCore_H_
#define BDN_ANDROID_TEST_testAndroidViewCore_H_


#include <bdn/test/testViewCore.h>

#include <bdn/View.h>
#include <bdn/Window.h>
#include <bdn/android/ViewCore.h>

namespace bdn
{
namespace android
{
namespace test
{

/** Tests the android view core that is associated with the specified view.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.
        */
inline void testAndroidViewCore(P<Window> pWindow, P<View> pView, bool canCalculatePreferredSize)
{
    SECTION("generic")
        bdn::test::testViewCore(pWindow, pView, canCalculatePreferredSize);

    SECTION("android-specific")
    {
        P<bdn::android::ViewCore> pCore = cast<bdn::android::ViewCore>( pView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        bdn::android::JView jv = pCore->getJView();

        REQUIRE( !jv.isNull_() );

        SECTION("visible")
        {
            if(pView->visible().get())
                REQUIRE( jv.getVisibility()==bdn::android::JView::Visibility::visible );
            else
                REQUIRE( jv.getVisibility()==bdn::android::JView::Visibility::invisible );
            
            pView->visible() = true;
            REQUIRE( jv.getVisibility()==bdn::android::JView::Visibility::visible );

            pView->visible() = false;
            REQUIRE( jv.getVisibility()==bdn::android::JView::Visibility::invisible );

            pView->visible() = true;
            REQUIRE( jv.getVisibility()==bdn::android::JView::Visibility::visible );
        }

        
        SECTION("padding")
        {
            int defaultTop = jv.getPaddingTop();
            int defaultRight = jv.getPaddingRight();
            int defaultBottom = jv.getPaddingBottom();
            int defaultLeft = jv.getPaddingLeft();

            pView->padding() = UiMargin(UiLength::realPixel, 11, 22, 33, 44);

            REQUIRE( jv.getPaddingTop() == 11 );
            REQUIRE( jv.getPaddingRight() == 22 );
            REQUIRE( jv.getPaddingBottom() == 33 );
            REQUIRE( jv.getPaddingLeft() == 44 );

            pView->padding() = nullptr;

            REQUIRE( jv.getPaddingTop() == defaultTop );
            REQUIRE( jv.getPaddingRight() == defaultRight );
            REQUIRE( jv.getPaddingBottom() == defaultBottom );
            REQUIRE( jv.getPaddingLeft() == defaultLeft );
        }

        SECTION("bounds")
        {
            // bounds should translate 1:1, but require a few events
            // to be processed
            pView->bounds() = Rect(110, 220, 880, 990);

            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pWindow, pView, jv) mutable
            {
                REQUIRE( jv.getLeft()==110 );
                REQUIRE( jv.getTop()==220 );
                REQUIRE( jv.getWidth()==880 );
                REQUIRE( jv.getHeight()==990 );
            };
        }
    }
}


}
}
}


#endif

