#ifndef BDN_TEST_TestWindowCore_H_
#define BDN_TEST_TestWindowCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/Window.h>

namespace bdn
{
namespace test
{
    

/** Helper for tests that verify IWindowCore implementations.*/
class TestWindowCore : public TestViewCore<Window>
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
        P<TestWindowCore> pThis(this);
    
        TestViewCore::runPostInitTests();

        SECTION("title")
        {
            SECTION("value")
            {
                _pWindow->title() = "hello world";
                
                CONTINUE_SECTION_WHEN_IDLE(pThis)
                {
                    pThis->verifyCoreTitle();
                };
            }

            SECTION("does not affect preferred size")
            {                
                // the title should not affect the window's preferred size.
                Size prefSizeBefore = _pWindow->calcPreferredSize();

                _pWindow->title() = "this is a long long long long long long long long long long long long title";
                
                CONTINUE_SECTION_WHEN_IDLE(pThis, prefSizeBefore)
                {
                    Size prefSize = pThis->_pWindow->calcPreferredSize();
                    
                    REQUIRE( prefSize == prefSizeBefore);
                };
            }
        }

        
        SECTION("layout arranges content view")
        {
            P<Button> pChild = newObj<Button>();

            _pWindow->setContentView(pChild);

            // set a left/top margin for the child so that it is moved to the bottom right
            Margin margin( 11, 0, 0, 22 );
            pChild->margin() = UiMargin( margin.top, margin.right, margin.bottom, margin.left );

            // then autosize the window
            _pWindow->requestAutoSize();

            P<TestWindowCore> pThis = this;

            BDN_CONTINUE_SECTION_WHEN_IDLE( pThis, pChild, margin)
            {
                Point oldPos = pChild->position();
                Size  oldSize = pChild->size();

                // then invert the margin and make the top margin a bottom margin and the
                // left margin a right margin
                pChild->margin() = UiMargin( 0, margin.left, margin.top, 0);
                

                // this should cause a layout. We know the layout happens (we test that in another case).
                // Here we only verify that the layout actually updates the content view.
                BDN_CONTINUE_SECTION_WHEN_IDLE( pThis, pChild, oldPos, oldSize, margin)
                {
                    // if a layout was done then the child position should now be moved to the left and up
                    // by the amount of the removed margin.
                    // The position might not match exactly, since it is rounded to full pixels
                    Point expectedPos(oldPos.x - margin.left, oldPos.y - margin.top);
                    Point pos = pChild->position();
                    REQUIRE_ALMOST_EQUAL( pos.x, expectedPos.x, 2);
                    REQUIRE_ALMOST_EQUAL( pos.y, expectedPos.y, 2);
                    
                    // size should not have changed
                    REQUIRE( pChild->size().get() == oldSize );
                };
                
            };
        }
                
        
        SECTION("Ui element destroyed when object destroyed")
        {
            // there may be pending sizing info updates for the window, which keep it alive.
            // Ensure that those are done first.

            CONTINUE_SECTION_WHEN_IDLE(pThis)
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

