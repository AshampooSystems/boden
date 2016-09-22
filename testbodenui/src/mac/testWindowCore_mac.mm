#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/ChildViewCore.hh>

using namespace bdn;


class TestMacWindowCore : public bdn::test::TestWindowCore
{
protected:


    void runPostInitTests() override
    {
        bdn::test::TestWindowCore::runPostInitTests();
        
        SECTION("content parent is flipped")
        {
            // the window's content parent must have the flipped
            // flag set, so that the origin of the coordinate
            // system is the top-left.
            P<Button> pButton = newObj<Button>();
            _pWindow->setContentView(pButton);
            
            NSView* pChild = cast<bdn::mac::ChildViewCore>(pButton->getViewCore())->getNSView();
            
            REQUIRE( pChild.superview.flipped );
        }
    }

    
    void initCore() override
    {
        TestWindowCore::initCore();
        
        _pMacWindowCore = cast<bdn::mac::WindowCore>( _pView->getViewCore() );
        REQUIRE( _pMacWindowCore!=nullptr );
        
        _pNSWindow = _pMacWindowCore->getNSWindow();
        REQUIRE( _pNSWindow!=nullptr );
    }
    
    IUiProvider& getUiProvider() override
    {
        return bdn::mac::UiProvider::get();
    }
    
    void verifyCoreVisibility() override
    {
        bool expectedVisible = _pView->visible();
        
        REQUIRE( _pNSWindow.visible == expectedVisible );
    }
    
    bdn::Rect getFrameRect() const
    {
        NSScreen*   screen = _pNSWindow.screen;
        if(screen==nil) // happens when window is not visible
            screen = [NSScreen mainScreen];
        
        return bdn::mac::macRectToRect( _pNSWindow.frame, screen.frame.size.height);
    }
    
    void verifyInitialDummyCorePosition() override
    {
        bdn::Rect rect = getFrameRect();
        
        REQUIRE( rect.getPosition() == bdn::Point() );
    }

	void verifyInitialDummyCoreSize() override
    {
        bdn::Rect rect = getFrameRect();
        
        REQUIRE( rect.getSize() == bdn::Size() );
    }
    
   
	void verifyCorePosition() override
    {
        bdn::Rect	rect = getFrameRect();
        bdn::Point	expectedPosition = _pView->position();
        
        REQUIRE( rect.getPosition() == expectedPosition );
    }
    
	 void verifyCoreSize() override
    {
        bdn::Rect rect = getFrameRect();
        bdn::Size expectedSize = _pView->size();
        
        REQUIRE( rect.getSize() == expectedSize );
    }

    
    void verifyCorePadding() override
    {
        // the padding is not reflected in Cocoa properties.
        // So nothing to test here.
    }
    
    
    
    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();
        
        String title = bdn::mac::macStringToString( _pNSWindow.title );
        
        REQUIRE( title == expectedTitle );
    }
    
    
    
    void clearAllReferencesToCore() override
    {
        TestWindowCore::clearAllReferencesToCore();
        
        _pMacWindowCore = nullptr;
        _pNSWindow = nullptr;
    }
    
    
    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(NSWindow* pNSWindow)
        {
            this->_pNSWindow = pNSWindow;
        }
        
        // store a weak reference so that we do not keep the window alive
        NSWindow __weak* _pNSWindow;
    };
    
    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        // sanity check
        REQUIRE( _pNSWindow!=nullptr );
        
        return newObj<DestructVerificationInfo>( _pNSWindow );
    }
    
    void verifyCoreUiElementDestruction(IBase* pVerificationInfo) override
    {
        NSWindow __weak* pNSWindow = cast<DestructVerificationInfo>( pVerificationInfo )->_pNSWindow;
        
        // window should have been destroyed.
        REQUIRE( pNSWindow == nullptr );
    }

    P<bdn::mac::WindowCore>  _pMacWindowCore;
    NSWindow*                _pNSWindow;
};


TEST_CASE("mac.WindowCore")
{
    P<TestMacWindowCore> pTest = newObj<TestMacWindowCore>();
    
    pTest->runTests();
}





