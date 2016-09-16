#ifndef BDN_TEST_TestWindowCore_H_
#define BDN_TEST_TestWindowCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/Window.h>

namespace bdn
{
namespace test
{
    

/** Helper for tests that verify IWindowCore implementations.*/
class TestWindowCore : public TestViewCore
{

protected:

    bool coreCanCalculatePreferredSize() override
    {
        // Window cores cannot calculate a preferred size
        return false;
    }

    P<View> createView() override
    {
        return _pWindow;
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);
    }

    void initCore() override
    {
        TestViewCore::initCore();

        _pWindow->visible() = true;

        _pWindowCore = cast<IWindowCore>( _pCore );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();

        SECTION("title")
        {
            _pWindow->title() = "hello world";

            initCore();
            verifyCoreTitle();
        }
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();

        SECTION("title")
        {
            SECTION("value")
            {
                _pWindow->title() = "hello world";
                verifyCoreTitle();
            }

            SECTION("does not affect preferred size")
            {                
                // the title should not affect the window's preferred size.
                Size prefSizeBefore = _pWindow->calcPreferredSize();

                _pWindow->title() = "this is a long long long long long long long long long long long long title";

                Size prefSize = _pWindow->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore);
            }
        }

        
        SECTION("contentArea")
        {
            // make the window a somewhat big size.
            // Note that fullscreen windows may ignore this, but that is ok.
            // We only want to avoid cases where the window is tiny.
            _pWindow->bounds() = Rect(0, 0, 1000, 1000);

            P<TestWindowCore> pThis = this;

            // continue async to give the core a chance to correct / override
            // the new bounds.
            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pThis)
            {
                Rect bounds = pThis->_pWindow->bounds();

                Rect contentArea = pThis->_pWindowCore->getContentArea();

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
                Size windowSize = _pWindowCore->calcWindowSizeFromContentAreaSize( Size() );
                REQUIRE( windowSize.width>=0 );
                REQUIRE( windowSize.height>=0 );
            }

            SECTION("nonzero")
            {
                Size contentSize(1000, 2000);
                Size windowSize = _pWindowCore->calcWindowSizeFromContentAreaSize( contentSize );
                REQUIRE( windowSize.width>=contentSize.width );
                REQUIRE( windowSize.height>=contentSize.height );
            }
        }


        SECTION("calcContentAreaSizeFromWindowSize")
        {
            SECTION("zero")
            {
                // should never get a negative size
                REQUIRE( _pWindowCore->calcContentAreaSizeFromWindowSize( Size() ) == Size() );
            }

            SECTION("nonzero")
            {
                Size windowSize(1000, 2000);
                Size contentSize = _pWindowCore->calcContentAreaSizeFromWindowSize( windowSize );
                REQUIRE( contentSize.width>0 );
                REQUIRE( contentSize.height>0 );
                REQUIRE( contentSize.width<=windowSize.width );
                REQUIRE( contentSize.height<=windowSize.height );
            }
        }
    
        SECTION("calcMinimumSize")
        {
            Size minSize = _pWindowCore->calcMinimumSize();

            REQUIRE( minSize.width>=0 );
            REQUIRE( minSize.height>=0 );
        }

        SECTION("getScreenWorkArea")
        {
            Rect area = _pWindowCore->getScreenWorkArea();

            // note that the work area may have negative coordinates.
            REQUIRE( area.width>0 );
            REQUIRE( area.height>0 );
        }

        
        SECTION("Ui element destroyed when object destroyed")
        {
            // there may be pending sizing info updates for the window, which keep it alive.
            // Ensure that those are done first.

            P<TestWindowCore> pThis = this;

            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pThis)
            {
                pThis->testCoreUiElementDestroyedWhenObjectDestroyed();                
            };
        }
    }


    /** Implementations must return an object with the necessary information to be able to verify later
        that the core Ui element for the window was destroyed (see verifyCoreUiElementDestruction() ).
        
        The returned object must not hold a reference to the Window object or the core object.
        
        */
    virtual P<IBase> createInfoToVerifyCoreUiElementDestruction()=0;


    /** Verify that the core UI element of the window was destroyed.
        
        The outer Window object and possible also the core object have already been
        destroyed at this point.
    
        pVerificationInfo is the object with the verification information that was returned by an earlier
        call to createInfoToVerifyCoreUiElementDestruction().        
        */
    virtual void verifyCoreUiElementDestruction(IBase* pVerificationInfo)=0;


    /** Removes all references to the outer window object, causing it to be destroyed.*/
    virtual void clearAllReferencesToOuterWindow()
    {
        _pView = nullptr;
        _pWindow = nullptr;
    }

    /** Removes all references to the core object.*/
    virtual void clearAllReferencesToCore()
    {
        _pCore = nullptr;
        _pWindowCore = nullptr;
    }


    void testCoreUiElementDestroyedWhenObjectDestroyed()
    {
        P<IBase> pVerifyInfo = createInfoToVerifyCoreUiElementDestruction();
        
        clearAllReferencesToOuterWindow();
        
        P<IViewCore> pCoreKeepAlive;
        
       
        SECTION("core not kept alive")
        {
            // do nothing
        }

        SECTION("core kept alive")
        {
            pCoreKeepAlive = _pCore;
        }
        
        clearAllReferencesToCore();
        
        P<TestWindowCore> pThis = this;
        
        // it may be that deleted windows are garbage collected.
        // So we wait a few seconds before we check if the window is gone
        CONTINUE_SECTION_AFTER_SECONDS(2, pThis, pVerifyInfo )
        {
            pThis->verifyCoreUiElementDestruction(pVerifyInfo);
        };
    }
    
    /** Verifies that the window core's title has the expected value
        (the title set in the outer window object's Window::title() property.*/
    virtual void verifyCoreTitle()=0;


    P<IWindowCore>  _pWindowCore;
};




}
}

#endif

