#ifndef BDN_WIN32_TEST_testWin32ViewCore_H_
#define BDN_WIN32_TEST_testWin32ViewCore_H_


#include <bdn/test/testViewCore.h>

#include <bdn/View.h>
#include <bdn/win32/ViewCore.h>

namespace bdn
{
namespace win32
{
namespace test
{

/** Tests the win32 view core that is associated with the specified view.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.
        */
inline void testWin32ViewCore(P<Window> pWindow, P<View> pView, bool canCalculatePreferredSize)
{
    SECTION("generic")
        bdn::test::testViewCore(pWindow, pView, canCalculatePreferredSize);

    SECTION("win32-specific")
    {
        P<bdn::win32::ViewCore> pCore = cast<bdn::win32::ViewCore>( pView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        HWND hwnd = pCore->getHwnd();

        REQUIRE( hwnd!=NULL );

        SECTION("visible")
        {
            REQUIRE( ((::GetWindowLong( hwnd, GWL_STYLE) & WS_VISIBLE)==WS_VISIBLE) == pView->visible().get() );
            
            pView->visible() = true;
            REQUIRE( (::GetWindowLong( hwnd, GWL_STYLE) & WS_VISIBLE)==WS_VISIBLE );

            pView->visible() = false;
            REQUIRE( (::GetWindowLong( hwnd, GWL_STYLE) & WS_VISIBLE)!=WS_VISIBLE );

            pView->visible() = true;
            REQUIRE( (::GetWindowLong( hwnd, GWL_STYLE) & WS_VISIBLE)==WS_VISIBLE );
        }

        
        SECTION("padding")
        {
            // padding is not translated to a win32 HWND property.
            // So there is nothing to test for it.
        }

        SECTION("bounds")
        {
            // bounds should translate 1:1
            pView->bounds() = Rect(110, 220, 880, 990);


            int x=-1;
            int y=-1;
            int width=-1;
            int height=-1;

            {
                RECT winRect{};
                ::GetWindowRect(hwnd, &winRect);

                x = winRect.left;
                y = winRect.top;
                width = winRect.right-winRect.left;
                height = winRect.bottom-winRect.top;
            
                HWND parentHwnd = ::GetParent(hwnd);
                if(parentHwnd!=NULL)
                {
                    POINT pos { x, y };

                    ::ScreenToClient( parentHwnd, &pos);

                    x = pos.x;
                    y = pos.y;
                }
            }
            
            REQUIRE( x == 110 );
            REQUIRE( y == 220 );
            REQUIRE( width == 880 );
            REQUIRE( height == 990 );
        }

        SECTION("tryChangeParent")
        {
            P<Window> pOtherWindow = newObj<Window>( &bdn::win32::UiProvider::get() );

            // in win32 one cannot change the parent. So this should always fail
            bool result = pCore->tryChangeParentView( pOtherWindow );
            REQUIRE( result==false );

            // the core should still have its old window
            REQUIRE( pCore->getHwnd() == hwnd );
        }
    }
}


}
}
}


#endif

