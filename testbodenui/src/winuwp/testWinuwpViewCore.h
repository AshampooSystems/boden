#ifndef BDN_WINUWP_TEST_testWinuwpViewCore_H_
#define BDN_WINUWP_TEST_testWinuwpViewCore_H_


#include <bdn/test/testViewCore.h>

#include <bdn/View.h>
#include <bdn/winuwp/IFrameworkElementOwner.h>

namespace bdn
{
namespace winuwp
{
namespace test
{

/** Tests the winuwp view core that is associated with the specified view.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.
        */
inline void testWinuwpViewCore(P<Window> pWindow, P<View> pView, bool canCalculatePreferredSize, bool canUpdateBounds)
{
    SECTION("generic")
        bdn::test::testViewCore(pWindow, pView, canCalculatePreferredSize);

    SECTION("winuwp-specific")
    {
        P<bdn::winuwp::IFrameworkElementOwner> pCore = cast<bdn::winuwp::IFrameworkElementOwner>( pView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        ::Windows::UI::Xaml::FrameworkElement^ pEl = pCore->getFrameworkElement();
        REQUIRE( pEl!=nullptr );
        
        SECTION("visible")
        {
            REQUIRE( pEl->Visibility == (pView->visible().get() ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed) );
            
            pView->visible() = true;
            REQUIRE( pEl->Visibility == Windows::UI::Xaml::Visibility::Visible );

            pView->visible() = false;
            REQUIRE( pEl->Visibility == Windows::UI::Xaml::Visibility::Collapsed );

            pView->visible() = true;
            REQUIRE( pEl->Visibility == Windows::UI::Xaml::Visibility::Visible );
        }

        
        SECTION("padding")
        {
            pView->padding() = UiMargin( UiLength::pixel96, 11, 22, 33, 44);

            ::Windows::UI::Xaml::Thickness pad;
            if( dynamic_cast<::Windows::UI::Xaml::Controls::Control^>(pEl)!=nullptr)
                pad = dynamic_cast<::Windows::UI::Xaml::Controls::Control^>(pEl)->Padding;
            else if( dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>(pEl)!=nullptr)
                pad = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>(pEl)->Padding;
            else
            {
                REQUIRE(false ); // unknown control type
            }

            double scaleFactor = bdn::winuwp::UiProvider::get().getUiScaleFactor();
    
            REQUIRE_ALMOST_EQUAL( pad.Top, 11, 1 );
            REQUIRE_ALMOST_EQUAL( pad.Right, 22, 1 );
            REQUIRE_ALMOST_EQUAL( pad.Bottom, 33, 1 );
            REQUIRE_ALMOST_EQUAL( pad.Left, 44, 1 );
        }

        if(canUpdateBounds)
        {
            SECTION("bounds")
            {
                // bounds should translate 1:1
                pView->bounds() = Rect(110, 220, 880, 990);

                double x = ::Windows::UI::Xaml::Controls::Canvas::GetLeft(pEl);
		        double y = ::Windows::UI::Xaml::Controls::Canvas::GetTop(pEl);
                double width = pEl->Width;
                double height = pEl->Height;

                double scaleFactor = bdn::winuwp::UiProvider::get().getUiScaleFactor();

                REQUIRE_ALMOST_EQUAL( x, 110 / scaleFactor, 1 );
                REQUIRE_ALMOST_EQUAL( y, 220 / scaleFactor, 1 );
                REQUIRE_ALMOST_EQUAL( width, 880 / scaleFactor, 1 );
                REQUIRE_ALMOST_EQUAL( height, 990 / scaleFactor, 1 );
            }
        }

    }
}




/** Tests that the initialization of the winuwp view core for the specified view is performed correctly.

    The view MUST NOT yet be added to a container or window. The test function will add it
    to the specified window dynamically.

    */
inline void testWinuwpViewCoreInitialization(P<Window> pWindow, P<View> pView)
{
    // view must not yet have a parent - otherwise this test will not test the desired effects
    REQUIRE( pView->getParentView()==nullptr );

    SECTION("visible")
        pView->visible() = true;
    
    SECTION("invisible")
        pView->visible() = false;


    SECTION("padding")
    {
        pView->padding() = UiMargin( UiLength::pixel96, 11, 22, 33, 44);
    }

    SECTION("bounds")
        pView->bounds() = Rect(110, 220, 880, 990);

    bool expectedVisible = pView->visible();
    Nullable<UiMargin> expectedPadding = pView->padding();

    // now add the view to the window. This will cause the core to be created.
    pWindow->setContentView( pView );

    // and now we verify that the win32 window's properties are as expected
    P<bdn::winuwp::IFrameworkElementOwner> pCore = cast<bdn::winuwp::IFrameworkElementOwner>( pView->getViewCore() );
    REQUIRE( pCore!=nullptr );

    ::Windows::UI::Xaml::FrameworkElement^ pEl = pCore->getFrameworkElement();
    REQUIRE( pEl!=nullptr );

    REQUIRE( pEl->Visibility == (expectedVisible ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed) );
               
    double x = ::Windows::UI::Xaml::Controls::Canvas::GetLeft(pEl);
    double y = ::Windows::UI::Xaml::Controls::Canvas::GetTop(pEl);
    double width = pEl->Width;
    if(std::isnan(width))
        width = 0;
    double height = pEl->Height;
    if(std::isnan(height))
        height = 0;
    
    // the bounds should NEVER be initialized upon construction.
    // Instead they must be updated by a layout cycle after construction.
    // So at this point in time the bounds should be zero
            
    REQUIRE( x == 0 );
    REQUIRE( y == 0 );
    REQUIRE( width == 0 );
    REQUIRE( height == 0 );



    
    if(!expectedPadding.isNull())
    {
        Margin expectedPaddingPixels;

        expectedPaddingPixels = pView->uiMarginToPixelMargin( expectedPadding.get() );

        ::Windows::UI::Xaml::Thickness pad;
        if( dynamic_cast<::Windows::UI::Xaml::Controls::Control^>(pEl)!=nullptr)
            pad = dynamic_cast<::Windows::UI::Xaml::Controls::Control^>(pEl)->Padding;
        else if( dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>(pEl)!=nullptr)
            pad = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>(pEl)->Padding;
        else
        {
            REQUIRE(false ); // unknown control type
        }

        double scaleFactor = bdn::winuwp::UiProvider::get().getUiScaleFactor();
    
        REQUIRE_ALMOST_EQUAL( pad.Top, expectedPaddingPixels.top/scaleFactor, 1 );
        REQUIRE_ALMOST_EQUAL( pad.Right, expectedPaddingPixels.right/scaleFactor, 1 );
        REQUIRE_ALMOST_EQUAL( pad.Bottom, expectedPaddingPixels.bottom/scaleFactor, 1 );
        REQUIRE_ALMOST_EQUAL( pad.Left, expectedPaddingPixels.left/scaleFactor, 1 );
    }

}


}
}
}


#endif

