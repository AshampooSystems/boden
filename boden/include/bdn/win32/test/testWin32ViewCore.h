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
inline void testWin32ViewCore(View* pView, bool canCalculatePreferredSize)
{
    SECTION("generic")
        bdn::test::testViewCore(pView, canCalculatePreferredSize);

    SECTION("win32-specific")
    {
        P<bdn::win32::ViewCore> pCore = cast<bdn::win32::ViewCore>( pView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        HWND hwnd = pCore->getHwnd();

        REQUIRE( hwnd!=NULL );

        SECTION("visible")
        {
            pView->visible() = true;
            REQUIRE( ::IsWindowVisible(hwnd) );

            pView->visible() = false;
            REQUIRE( !::IsWindowVisible(hwnd) );

            pView->visible() = true;
            REQUIRE( ::IsWindowVisible(hwnd) );
        }

        
        SECTION("padding")
        {
            // padding is not translated to a win32 HWND property.
            // So there is nothing to test for it.
        }

        SECTION("bounds")
        {
            // bounds should translate 1:1
            pView->bounds() = Rect(11, 22, 33, 44);

            RECT winRect;
            ::GetWindowRect(hwnd, &winRect);
            REQUIRE( winRect.left == 11 );
            REQUIRE( winRect.top == 22 );
            REQUIRE( winRect.right == 11+33 );
            REQUIRE( winRect.bottom == 22+44 );
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

