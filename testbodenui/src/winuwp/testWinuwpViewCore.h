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
            // padding is not translated to a win32 HWND property.
            // So there is nothing to test for it.
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


}
}
}


#endif

