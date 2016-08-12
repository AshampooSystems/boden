#ifndef BDN_TEST_testWindowCore_H_
#define BDN_TEST_testWindowCore_H_

#include <bdn/test/testViewCore.h>
#include <bdn/Window.h>

namespace bdn
{
namespace test
{


/** Performs generic tests for the window core that is currently associated with the specified Window.

    Note that these tests cannot test the effects of some of the functions on the actual UI element
    implementation that the core accesses. So the unit tests for the concrete implementation should verify these
    effects in addition to executing these generic tests.
*/
inline void testWindowCore(P<Window> pWindow)
{
    P<IWindowCore> pCore = cast<IWindowCore>( pWindow->getViewCore() );
    REQUIRE( pCore!=nullptr );

    SECTION("ViewCore-base")
        testViewCore(pWindow, pWindow, false);


    SECTION("title")
    {
        // the title should not affect the window's preferred size.
        Size prefSizeBefore = pWindow->calcPreferredSize();

        pWindow->title() = "hello world";
        
        REQUIRE( pWindow->calcPreferredSize() == prefSizeBefore );
    }

    SECTION("contentArea")
    {
        // make the window a somewhat big size.
        // Note that fullscreen windows may ignore this, but that is ok.
        // We only want to avoid cases where the window is tiny.
        pWindow->bounds() = Rect(0, 0, 1000, 1000);

        // continue async to give the core a chance to correct / override
        // the new bounds.
        CONTINUE_SECTION_ASYNC(pWindow, pCore)
        {
            Rect bounds = pWindow->bounds();

            Rect contentArea = pCore->getContentArea();

            REQUIRE( contentArea.x>=0 );
            REQUIRE( contentArea.y>=0 );
            REQUIRE( contentArea.width>0 );
            REQUIRE( contentArea.height>0 );
            
            // the content area must be fully inside the window bounds.
            REQUIRE( contentArea.x + contentArea.width <= bounds.width);
            REQUIRE( contentArea.y + contentArea.height <= bounds.height);         
        };
    }

    SECTION("calcWindowSizeFromContentAreaSize")
    {
        SECTION("zero")
        {
            Size windowSize = pCore->calcWindowSizeFromContentAreaSize( Size() );
            REQUIRE( windowSize.width>=0 );
            REQUIRE( windowSize.height>=0 );
        }

        SECTION("nonzero")
        {
            Size contentSize(1000, 2000);
            Size windowSize = pCore->calcWindowSizeFromContentAreaSize( contentSize );
            REQUIRE( windowSize.width>=contentSize.width );
            REQUIRE( windowSize.height>=contentSize.height );
        }
    }


    SECTION("calcContentAreaSizeFromWindowSize")
    {
        SECTION("zero")
        {
            // should never get a negative size
            REQUIRE( pCore->calcContentAreaSizeFromWindowSize( Size() ) == Size() );
        }

        SECTION("nonzero")
        {
            Size windowSize(1000, 2000);
            Size contentSize = pCore->calcContentAreaSizeFromWindowSize( windowSize );
            REQUIRE( contentSize.width>0 );
            REQUIRE( contentSize.height>0 );
            REQUIRE( contentSize.width<=windowSize.width );
            REQUIRE( contentSize.height<=windowSize.height );
        }
    }
    
    SECTION("calcMinimumSize")
    {
        Size minSize = pCore->calcMinimumSize();

        REQUIRE( minSize.width>=0 );
        REQUIRE( minSize.height>=0 );
    }

    SECTION("getScreenWorkArea")
    {
        Rect area = pCore->getScreenWorkArea();

        // note that the work area may have negative coordinates.
        REQUIRE( area.width>0 );
        REQUIRE( area.height>0 );
    }


}

}
}

#endif

