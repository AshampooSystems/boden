#ifndef BDN_TEST_testViewCore_H_
#define BDN_TEST_testViewCore_H_

#include <bdn/View.h>
#include <bdn/test.h>

namespace bdn
{
namespace test
{


/** Performs generic tests for the view core that is currently associated with the specified View.

    Note that these tests cannot test the effects of some of the functions on the actual UI element
    implementation that the core accesses. So the unit tests for the concrete implementation should verify these
    effects in addition to executing these generic tests.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.

*/
inline void testViewCore(View* pView, bool canCalculatePreferredSize)
{
    P<IViewCore> pCore = pView->getViewCore();
    REQUIRE( pCore!=nullptr );


    SECTION("uiLengthToPixels")
    {
        REQUIRE( pCore->uiLengthToPixels( UiLength(UiLength::realPixel, 0) ) == 0 );
        REQUIRE( pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 0) ) == 0 );
        REQUIRE( pCore->uiLengthToPixels( UiLength(UiLength::sem, 0) ) == 0 );

        REQUIRE( pCore->uiLengthToPixels( UiLength(UiLength::realPixel, 17) ) == 17 );

        int semSize = pCore->uiLengthToPixels( UiLength(UiLength::sem, 1) );
        REQUIRE( semSize>0 );
        REQUIRE_ALMOST_EQUAL( pCore->uiLengthToPixels( UiLength(UiLength::sem, 3) ), semSize*3, 3);

        int pixel96Size = pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 1) );
        REQUIRE( pixel96Size>0 );
        REQUIRE_ALMOST_EQUAL( pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 3) ), pixel96Size*3, 3);
    }


    SECTION("uiMarginToPixelMargin")
    {
        SECTION("realPixel")
            REQUIRE( pCore->uiMarginToPixelMargin( UiMargin(UiLength::realPixel, 10, 20, 30, 40) ) == Margin(10, 20, 30, 40) );

        SECTION("sem")
        {
            int semSize = pCore->uiLengthToPixels( UiLength(UiLength::sem, 1) );

            Margin m = pCore->uiMarginToPixelMargin( UiMargin(UiLength::sem, 10, 20, 30, 40) );
            REQUIRE_ALMOST_EQUAL( m.top, 10*semSize, 10);
            REQUIRE_ALMOST_EQUAL( m.right, 20*semSize, 20);
            REQUIRE_ALMOST_EQUAL( m.bottom, 30*semSize, 30);
            REQUIRE_ALMOST_EQUAL( m.left, 40*semSize, 40);
        }

        SECTION("pixel96")
        {
            int pixel96Size = pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 1) );

            Margin m = pCore->uiMarginToPixelMargin( UiMargin(UiLength::pixel96, 10, 20, 30, 40) );
            REQUIRE_ALMOST_EQUAL( m.top, 10*pixel96Size, 10);
            REQUIRE_ALMOST_EQUAL( m.right, 20*pixel96Size, 20);
            REQUIRE_ALMOST_EQUAL( m.bottom, 30*pixel96Size, 30);
            REQUIRE_ALMOST_EQUAL( m.left, 40*pixel96Size, 40);
        }
    }
    
    SECTION("visible")   
    {
        // we cannot test much here, since we cannot access the actual
        // UI element behind the core. But we can test that there are no exceptions
        // or crashes.

        // we also test that visibility has no effect on the preferred size.
        Size prefSizeBefore;
        if(canCalculatePreferredSize)
            prefSizeBefore = pCore->calcPreferredSize();

        pView->visible() = true;
        if(canCalculatePreferredSize)
            REQUIRE( pCore->calcPreferredSize() == prefSizeBefore );

        pView->visible() = false;
        if(canCalculatePreferredSize)
            REQUIRE( pCore->calcPreferredSize() == prefSizeBefore );

        pView->visible() = true;
        if(canCalculatePreferredSize)
            REQUIRE( pCore->calcPreferredSize() == prefSizeBefore );
    }

    SECTION("padding")
    {
        Size prefSizeBefore;
        if(canCalculatePreferredSize)
            prefSizeBefore = pCore->calcPreferredSize();

        Margin padding(10, 20, 30, 40);

        // setting padding should increase the preferred size
        // of the core.
        pView->padding() = UiMargin(UiLength::realPixel, padding.top, padding.right, padding.bottom, padding.left);

        // the padding should increase the preferred size.
        if(canCalculatePreferredSize)
        {
            Size prefSize = pCore->calcPreferredSize();
            REQUIRE( prefSize == prefSizeBefore+padding );
        }
    }

    SECTION("bounds")
    {
        // test that the bounds do not influence the preferred size.
        Size prefSizeBefore;
        if(canCalculatePreferredSize)
            prefSizeBefore = pCore->calcPreferredSize();

        pView->bounds() = Rect(100, 200, 300, 400);
        if(canCalculatePreferredSize)
            REQUIRE( pCore->calcPreferredSize() == prefSizeBefore );

        pView->bounds() = Rect(1000, 2000, 3000, 4000);
        if(canCalculatePreferredSize)
            REQUIRE( pCore->calcPreferredSize() == prefSizeBefore );
    }

    if(canCalculatePreferredSize)
    {	
	    SECTION("calcPreferredSize")	
        {
            // we already checked that padding is properly included in the preferred size
            // So here we only check that the preferred size is "plausible"

            Size prefSize = pCore->calcPreferredSize();

            REQUIRE( prefSize.width>=0 );
            REQUIRE( prefSize.height>=0 );
        }


        SECTION("calcPreferredHeightForWidth")	
        {
            // this is difficult to test, since it depends heavily on what kind of view
            // we actually work with. Also, it is perfectly normal for different core implementations
            // to have different preferred size values for the same inputs.

            // So we can only test rough plausibility here.
            Size prefSize = pCore->calcPreferredSize();

            SECTION("unconditionalWidth")
            {
                // When we specify exactly the unconditional preferred width then we should get exactly the unconditional preferred height
                REQUIRE( pCore->calcPreferredHeightForWidth(prefSize.width) == prefSize.height );
            }

            SECTION("zero")
            {
                REQUIRE( pCore->calcPreferredHeightForWidth(0) > 0);
            }
        }

        SECTION("calcPreferredWidthForHeight")	
        {
            Size prefSize = pCore->calcPreferredSize();

            SECTION("unconditionalHeight")
                REQUIRE( pCore->calcPreferredWidthForHeight(prefSize.height) == prefSize.width );
        
            SECTION("zero")
                REQUIRE( pCore->calcPreferredWidthForHeight(0) > 0);
        }
    }

    SECTION("tryChangeParentView")
    {
        // this is pretty much impossible to test for the generic case. All return values are plausible
        // and there is no way to check if the core actually changed to the new parent.
        // So this test can only be properly implemented by tests for the concrete
        // core implementation, not by these generic base tests.
    }
}

}
}

#endif

