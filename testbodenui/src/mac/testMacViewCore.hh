#ifndef BDN_MAC_TEST_testMacViewCore_H_
#define BDN_MAC_TEST_testMacViewCore_H_


#include <bdn/test/testViewCore.h>

#include <bdn/View.h>

#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/WindowCore.hh>

namespace bdn
{
namespace mac
{
namespace test
{

/** Tests the mac view core that is associated with the specified view.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.
        */
inline void testMacViewCore(P<Window> pWindow, P<View> pView, bool canCalculatePreferredSize)
{
    SECTION("generic")
        bdn::test::testViewCore(pWindow, pView, canCalculatePreferredSize);

    SECTION("mac-specific")
    {
        // all mac views are derived from mac::ViewCore, except for Windows cores.
        if( tryCast<bdn::mac::WindowCore>( pView->getViewCore() )!=nullptr )
        {
            P<bdn::mac::WindowCore> pCore = tryCast<bdn::mac::WindowCore>( pView->getViewCore() );
            
            REQUIRE( pCore!=nullptr );
            
            NSWindow* pNS = pCore->getNSWindow();
            REQUIRE( pNS!=nullptr );
            
            SECTION("visible")
            {
                REQUIRE( pNS.visible == pView->visible().get() );
                
                pView->visible() = true;
                REQUIRE( pNS.visible );
                
                pView->visible() = false;
                REQUIRE( !pNS.visible );
                
                pView->visible() = true;
                REQUIRE( pNS.visible );
            }
            
            
            SECTION("padding")
            {
                // padding is not translated to a NSWindow property. It only affects the layout size.
                // So there is nothing mac-specific to test here.
            }
            
            SECTION("bounds")
            {
                // bounds should translate 1:1
                Rect newBounds(110, 220, 880, 990);
                pView->bounds() = newBounds;
                
                Rect viewFrame = macRectToRect( pNS.frame, pNS.screen.frame.size.height);
                
                REQUIRE( viewFrame == newBounds );
            }

            
        }
        else
        {
            P<bdn::mac::ViewCore> pCore = tryCast<bdn::mac::ViewCore>( pView->getViewCore() );
        
            REQUIRE( pCore!=nullptr );
            
            NSView* pNS = pCore->getNSView();
            REQUIRE( pNS!=nullptr );

            SECTION("visible")
            {
                REQUIRE( pNS.hidden == !pView->visible().get() );
                
                pView->visible() = true;
                REQUIRE( !pNS.hidden );

                pView->visible() = false;
                REQUIRE( pNS.hidden );

                pView->visible() = true;
                REQUIRE( !pNS.hidden );
            }

            
            SECTION("padding")
            {
                // padding is not translated to a NSView property. It only affects the layout size.
                // So there is nothing mac-specific to test here.
            }

            SECTION("bounds")
            {
                // bounds should translate 1:1
                Rect newBounds(110, 220, 880, 990);
                pView->bounds() = newBounds;
                
                Rect viewFrame = macRectToRect( pNS.frame, -1 );
                
                REQUIRE( viewFrame == newBounds );
            }
        }
    }
}


}
}
}


#endif

