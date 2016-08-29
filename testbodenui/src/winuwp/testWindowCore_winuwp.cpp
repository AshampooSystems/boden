#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/WindowCore.h>
#include "testWinuwpViewCore.h"

using namespace bdn;

TEST_CASE("WindowCore-winuwp")
{
    P<Window> pWindow = newObj<Window>( &bdn::winuwp::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("winuwp-view")
        bdn::winuwp::test::testWinuwpViewCore(pWindow, pWindow, false, false);

    SECTION("winuwp-window")
    {
        P<bdn::winuwp::WindowCore> pCore = cast<bdn::winuwp::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        ::Windows::UI::Xaml::FrameworkElement^ pEl = dynamic_cast< ::Windows::UI::Xaml::FrameworkElement^ >(pCore->getFrameworkElement());
        REQUIRE( pEl!=nullptr );

        SECTION("title")
        {
            // setTitle currently has no effect.
        }                
    }

    // XXX test disabled. For some reason the Parent property is always null,
    // so we cannot test for the destruction.
    /*SECTION("Xaml window destroyed when object destroyed")
    {
        // there may be pending sizing info updates for the window, which keep it alive.
        // Ensure that those are done first.

        pWindow->visible() = true;

        // wrap pWindow in a struct so that we can destroy all references
        // in the continuation.
        struct CaptureData : public Base
        {
            P<Window> pWindow;
        };
        P<CaptureData> pData = newObj<CaptureData>();
        pData->pWindow = pWindow;
        pWindow = nullptr;

        CONTINUE_SECTION_AFTER_PENDING_EVENTS(pData)
        {
            P<bdn::winuwp::WindowCore> pCore = cast<bdn::winuwp::WindowCore>( pData->pWindow->getViewCore() );
            REQUIRE( pCore!=nullptr );

            ::Windows::UI::Xaml::FrameworkElement^ pEl = pCore->getFrameworkElement();
            REQUIRE( pEl!=nullptr );

            // note that we cannot actually test whether the object is deleted or not.
            // That is done by the garbage collector at some point in time that we cannot predict.
            // But we CAN check if the panel is removed from the parent.
            
            ::Windows::UI::Xaml::DependencyObject^ pParent = pEl->Parent;
            REQUIRE( pParent != nullptr );


            SECTION("core not kept alive")
            {
                pCore = nullptr;
            }

            SECTION("core kept alive")
            {
                // do nothing
            }        

            pData->pWindow = nullptr;

            // the parent might be updated asynchronously. So process events before we continue.

            // give the WinRT background updater some time to do its work
            Thread::sleepSeconds(2);

            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pData, pEl)
            {
                // window panel should have been removed from the parent
                ::Windows::UI::Xaml::DependencyObject^ pParent = pEl->Parent;
                REQUIRE( pParent == nullptr );
            };
        };
    }*/
}




