#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>

#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/WindowCore.hh>
#import "testIosViewCore.hh"

using namespace bdn;

TEST_CASE("WindowCore-ios")
{
    P<Window> pWindow = newObj<Window>( &bdn::ios::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("ios-view")
        bdn::ios::test::testIosViewCore(pWindow, pWindow, false, false);

    SECTION("ios-window")
    {
        P<bdn::ios::WindowCore> pCore = cast<bdn::ios::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        UIWindow* pUIWindow = pCore->getUIWindow();
        REQUIRE( pUIWindow!=nullptr );

        SECTION("title")
        {
            // setTitle should change the window test
            pWindow->title() = "hello world";
            
            String text = bdn::ios::iosStringToString( pUIWindow.rootViewController.title );
            
            REQUIRE( text == "hello world" );
        }
    }
    
    
    /* XXX test disabled. Someone still holds a reference to the UIWindow, even after
       we have released our last reference. So we cannot test window deletion until we find
       out where those refs are stored. It is probably some global window registry
       in ios.
    SECTION("Window deleted when object destroyed")
    {
        // there may be pending sizing info updates for the window, which keep it alive.
        // Ensure that those are done first.
        
        // wrap pWindow in a struct so that we can destroy all references
        // in the continuation.
        struct CaptureData : public Base
        {
            P<Window> pWindow;
            P<Window> pReplacementWindow;
        };
        P<CaptureData> pData = newObj<CaptureData>();
        pData->pWindow = pWindow;
        pWindow = nullptr;
        
        // Before a window will be completely deleted by the OS there has to be
        // a new one to replace it. Otherwise the other one will not be completely
        // deleted.
        pData->pReplacementWindow = newObj<Window>( &bdn::ios::UiProvider::get() );
        pData->pReplacementWindow->visible() = true;

        CONTINUE_SECTION_ASYNC(pData)
        {
            P<bdn::ios::WindowCore> pCore = cast<bdn::ios::WindowCore>( pData->pWindow->getViewCore() );
            REQUIRE( pCore!=nullptr );
        
            __weak UIWindow* pUIWindow = pCore->getUIWindow();
            REQUIRE( pUIWindow!=nullptr );
            
            int windowRefCount = pData->pWindow->getRefCount();
            
            int arcRefCount = CFGetRetainCount((__bridge CFTypeRef)pUIWindow);
            
            pCore = nullptr;
            pData->pWindow = nullptr;
            
            arcRefCount = CFGetRetainCount((__bridge CFTypeRef)pUIWindow);
            
            CONTINUE_SECTION_ASYNC(pData, pUIWindow)
            {
            int arcRefCount = CFGetRetainCount((__bridge CFTypeRef)pUIWindow);
            
                REQUIRE( pUIWindow==nullptr );
            };
        };
    }*/
}





