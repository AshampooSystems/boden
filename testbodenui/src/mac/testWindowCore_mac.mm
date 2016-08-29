#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>

#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/test/testMacViewCore.hh>

using namespace bdn;

TEST_CASE("WindowCore-mac")
{
    P<Window> pWindow = newObj<Window>( &bdn::mac::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("mac-view")
        bdn::mac::test::testMacViewCore(pWindow, pWindow, false);

    SECTION("mac-window")
    {
        P<bdn::mac::WindowCore> pCore = cast<bdn::mac::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        NSWindow* pNS = pCore->getNSWindow();
        REQUIRE( pNS!=nullptr );

        SECTION("title")
        {
            // setTitle should change the window test
            pWindow->title() = "hello world";
            
            String text = bdn::mac::macStringToString( pNS.title );
            
            REQUIRE( text == "hello world" );
        }
    }
    
    /*  XXX test disabled. Someone still holds a reference to the NSWindow, even after
        we have released our last reference. So we cannot test window deletion until we find
        out where those refs are stored. It is probably some global window registry
        in the OS.
    SECTION("Window deleted when object destroyed")
    {
        // there may be pending sizing info updates for the window, which keep it alive.
        // Ensure that those are done first.
        
        // wrap pWindow in a struct so that we can destroy all references
        // in the continuation.
        struct CaptureData : public Base
        {
            P<Window> pWindow;
        };
        P<CaptureData> pData = newObj<CaptureData>();
        pData->pWindow = pWindow;
        pWindow = nullptr;
        
        
        CONTINUE_SECTION_ASYNC(pData)
        {
            P<bdn::mac::WindowCore> pCore = cast<bdn::mac::WindowCore>( pData->pWindow->getViewCore() );
            REQUIRE( pCore!=nullptr );
            
            __weak NSWindow* pNSWindow = pCore->getNSWindow();
            REQUIRE( pNSWindow!=nullptr );
            
            int arcRefCount = CFGetRetainCount((__bridge CFTypeRef)pNSWindow);
            
            pCore = nullptr;
            pData->pWindow = nullptr;
            
            arcRefCount = CFGetRetainCount((__bridge CFTypeRef)pNSWindow);
            
            REQUIRE( pNSWindow==nullptr );
        };
    }*/
}




