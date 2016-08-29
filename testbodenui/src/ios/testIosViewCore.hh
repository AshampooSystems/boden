#ifndef BDN_IOS_TEST_testIosViewCore_H_
#define BDN_IOS_TEST_testIosViewCore_H_


#include <bdn/test/testViewCore.h>

#include <bdn/View.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
namespace ios
{
namespace test
{

/** Tests the ios view core that is associated with the specified view.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.
        */
inline void testIosViewCore(P<Window> pWindow, P<View> pView, bool canCalculatePreferredSize, bool canModifyBounds)
{
    SECTION("generic")
        bdn::test::testViewCore(pWindow, pView, canCalculatePreferredSize);

    SECTION("ios-specific")
    {
        // all ios views are derived from ios::ViewCore
        P<bdn::ios::ViewCore> pCore = cast<bdn::ios::ViewCore>( pView->getViewCore() );
    
        REQUIRE( pCore!=nullptr );
        
        UIView* pUI = pCore->getUIView();
        REQUIRE( pUI!=nullptr );

        SECTION("visible")
        {
            REQUIRE( pUI.hidden == !pView->visible().get() );
            
            pView->visible() = true;
            REQUIRE( !pUI.hidden );

            pView->visible() = false;
            REQUIRE( pUI.hidden );

            pView->visible() = true;
            REQUIRE( !pUI.hidden );
        }

        
        SECTION("padding")
        {
            // padding is not translated to a NSView property. It only affects the layout size.
            // So there is nothing mac-specific to test here.
        }

        SECTION("bounds")
        {
            if(canModifyBounds)
            {
                Rect newBounds(110, 220, 880, 990);
                pView->bounds() = newBounds;
                
                Rect viewFrame = iosRectToRect( pUI.frame );
                
                // frame should have been restored
                REQUIRE( viewFrame == newBounds );
            }
            else
            {
                // bounds cannot be modified. The core should restore the old value.
                Rect oldBounds = pView->bounds();
                
                Rect newBounds(110, 220, 880, 990);
                pView->bounds() = newBounds;
                
                Rect viewFrame = iosRectToRect( pUI.frame );
                
                // frame should have been restored
                REQUIRE( viewFrame == oldBounds );
            }
        }
    }
}


}
}
}


#endif

