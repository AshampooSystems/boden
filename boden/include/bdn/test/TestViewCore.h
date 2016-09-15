#ifndef BDN_TEST_TestViewCore_H_
#define BDN_TEST_TestViewCore_H_

#include <bdn/View.h>
#include <bdn/Window.h>
#include <bdn/test.h>
#include <bdn/IUiProvider.h>
#include <bdn/RequireNewAlloc.h>
#include <bdn/Button.h>

namespace bdn
{
namespace test
{


/** Helper for tests that verify IViewCore implementations.*/
class TestViewCore : public RequireNewAlloc<Base, TestViewCore>
{
public:

    /** Performs the tests.*/
    virtual void runTests()
    {
        _pWindow = newObj<Window>( &getUiProvider() );

        _pWindow->visible() = true;

        setView( createView() );

        // sanity check: the view should not have a parent yet
        REQUIRE( _pView->getParentView()==nullptr );

        SECTION("init")
        {      
            if( _pView == cast<View>(_pWindow) )
            {
                // the view is a window. These always have a core from
                // the start, so we cannot do any init tests with them.

                // only check that the view core is indeed already there.
                REQUIRE(_pView->getViewCore()!=nullptr );
            }
            else
            {
                // non-windows should not have a view core in the beginning
                // (before they are added to the window).

                REQUIRE(_pView->getViewCore()==nullptr );

                // run the init tests for them
                runInitTests();
            }
        }

        SECTION("postInit")
        {
            initCore();

            // view should always be visible for these tests
            _pView->visible() = true;

            // ensure that all pending initializations have finished.
            P<TestViewCore> pThis = this;
            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pThis)
            {
                pThis->runPostInitTests();
            };
        }
    }    

protected:

    /** Returns true if the bounds of the view can be manually changed.
        Returns false if this is a UI element whose size and position are controlled
        by an external entity.
        
        The default implementation returns true
        */
    virtual bool canManuallyChangeBounds() const
    {
        return true;
    }

    /** Runs the tests that verify that the core initializes itself with the current
        property values of the outer view.

        The core is not yet initialized when this function is called
        
        The tests each modify an outer view property, then cause the core to be created
        (by calling initCore()) and then verify that the core has initialized itself correctly.
        */
    virtual void runInitTests()
    {
        SECTION("visible")
        {
            _pView->visible() = true;

            initCore();
            verifyCoreVisibility();
        }
    
        SECTION("invisible")
        {
            _pView->visible() = false;

            initCore();
            verifyCoreVisibility();
        }

        SECTION("padding")
        {
            SECTION("default")
            {
                // the default padding of the outer view should be null (i.e. "use default").
                REQUIRE( _pView->padding().get().isNull() );

                initCore();
                verifyCorePadding();
            }

            SECTION("explicit")
            {
                _pView->padding() = UiMargin( UiLength::sem, 11, 22, 33, 44);

                initCore();
                verifyCorePadding();
            }
        }

        SECTION("bounds")
        {
            _pView->bounds() = Rect(110, 220, 880, 990);

            initCore();
            verifyInitialDummyCoreBounds();
        }
    }

    /** Runs the tests that verify the core behaviour for operations that happen
        after the core is initialized.
        
        The core is already created/initialized when this is function is called.
        */
    virtual void runPostInitTests()
    {        
        SECTION("uiLengthToPixels")
        {
            REQUIRE( _pCore->uiLengthToPixels( UiLength(UiLength::realPixel, 0) ) == 0 );
            REQUIRE( _pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 0) ) == 0 );
            REQUIRE( _pCore->uiLengthToPixels( UiLength(UiLength::sem, 0) ) == 0 );

            REQUIRE( _pCore->uiLengthToPixels( UiLength(UiLength::realPixel, 17) ) == 17 );

            int semSize = _pCore->uiLengthToPixels( UiLength(UiLength::sem, 1) );
            REQUIRE( semSize>0 );
            REQUIRE_ALMOST_EQUAL( _pCore->uiLengthToPixels( UiLength(UiLength::sem, 3) ), semSize*3, 3);

            int pixel96Size = _pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 1) );
            REQUIRE( pixel96Size>0 );
            REQUIRE_ALMOST_EQUAL( _pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 3) ), pixel96Size*3, 3);
        }

        SECTION("uiMarginToPixelMargin")
        {
            SECTION("realPixel")
                REQUIRE( _pCore->uiMarginToPixelMargin( UiMargin(UiLength::realPixel, 10, 20, 30, 40) ) == Margin(10, 20, 30, 40) );

            SECTION("sem")
            {
                int semSize = _pCore->uiLengthToPixels( UiLength(UiLength::sem, 1) );

                Margin m = _pCore->uiMarginToPixelMargin( UiMargin(UiLength::sem, 10, 20, 30, 40) );
                REQUIRE_ALMOST_EQUAL( m.top, 10*semSize, 10);
                REQUIRE_ALMOST_EQUAL( m.right, 20*semSize, 20);
                REQUIRE_ALMOST_EQUAL( m.bottom, 30*semSize, 30);
                REQUIRE_ALMOST_EQUAL( m.left, 40*semSize, 40);
            }

            SECTION("pixel96")
            {
                int pixel96Size = _pCore->uiLengthToPixels( UiLength(UiLength::pixel96, 1) );

                Margin m = _pCore->uiMarginToPixelMargin( UiMargin(UiLength::pixel96, 10, 20, 30, 40) );
                REQUIRE_ALMOST_EQUAL( m.top, 10*pixel96Size, 10);
                REQUIRE_ALMOST_EQUAL( m.right, 20*pixel96Size, 20);
                REQUIRE_ALMOST_EQUAL( m.bottom, 30*pixel96Size, 30);
                REQUIRE_ALMOST_EQUAL( m.left, 40*pixel96Size, 40);
            }
        }

        
        if(coreCanCalculatePreferredSize())
        {	
            SECTION("preferredSize")
            {
	            SECTION("calcPreferredSize plausible")	
                {
                    // we check elsewhere that padding is properly included in the preferred size
                    // So here we only check that the preferred size is "plausible"

                    Size prefSize = _pCore->calcPreferredSize();

                    REQUIRE( prefSize.width>=0 );
                    REQUIRE( prefSize.height>=0 );
                }
                
                SECTION("calcPreferredSize restrictedWidth plausible")	
                {
                    // this is difficult to test, since it depends heavily on what kind of view
                    // we actually work with. Also, it is perfectly normal for different core implementations
                    // to have different preferred size values for the same inputs.

                    // So we can only test rough plausibility here.
                    Size prefSize = _pCore->calcPreferredSize();

                    SECTION("unconditionalWidth")
                    {
                        // When we specify exactly the unconditional preferred width then we should get exactly the unconditional preferred height
                        REQUIRE( _pCore->calcPreferredSize(prefSize.width).height == prefSize.height );
                    }

                    SECTION("zero")
                    {
                        REQUIRE( _pCore->calcPreferredSize(0).height >= prefSize.height );
                    }
                }

                SECTION("calcPreferredSize restrictedHeight plausible")	
                {
                    Size prefSize = _pCore->calcPreferredSize();

                    SECTION("unconditionalHeight")
                        REQUIRE( _pCore->calcPreferredSize(-1, prefSize.height).width == prefSize.width );
        
                    SECTION("zero")
                        REQUIRE( _pCore->calcPreferredSize(-1, 0).width >= prefSize.width );
                }
            }
        }
    
        SECTION("visibility")   
        {
            SECTION("visible")
            {
                _pView->visible() = true;
                verifyCoreVisibility();
            }

            SECTION("invisible")
            {
                _pView->visible() = false;
                verifyCoreVisibility();
            }

            if(coreCanCalculatePreferredSize())                
            {
                SECTION("noEffectOnPreferredSize")
                {
                    // verify that visibility has no effect on the preferred size
                    Size prefSizeBefore = _pCore->calcPreferredSize();

                    _pView->visible() = true;
                    REQUIRE( _pCore->calcPreferredSize() == prefSizeBefore );

                    _pView->visible() = false;
                    REQUIRE( _pCore->calcPreferredSize() == prefSizeBefore );

                    _pView->visible() = true;
                    REQUIRE( _pCore->calcPreferredSize() == prefSizeBefore );
                }
            }
        }

        SECTION("padding")
        {
            SECTION("custom")
            {
                _pView->padding() = UiMargin( UiLength::pixel96, 11, 22, 33, 44);
                verifyCorePadding();
            }

            SECTION("default after custom")
            {
                // set a non-default padding, then go back to default padding.
                _pView->padding() = UiMargin( UiLength::pixel96, 11, 22, 33, 44);
                _pView->padding() = nullptr;

                verifyCorePadding();
            }

            if(coreCanCalculatePreferredSize())
            {
                SECTION("effectsOnPreferredSize")
                {
                    // For some UI elements on some platforms there may be a silent minimum
                    // padding. If we specify a smaller padding then the minimum will be used
                    // instead.
        
                    // So to verify the effects of padding we first set a big padding that
                    // we are fairly confident to be over any minimum.
        
                    UiMargin paddingBefore(UiLength::sem, 10, 10, 10, 10);

                    _pView->padding() = paddingBefore;
                    
                    // wait a little so that sizing info is updated.
                    // Note that on some platforms CONTINUE_SECTION_AFTER_PENDING_EVENTS is not good enough
                    // because the sizing updates happen with a low priority.
                    // So we explicitly wait a little bit.
                    P<TestViewCore> pThis = this;
                    CONTINUE_SECTION_AFTER_SECONDS(0.5, pThis, paddingBefore )
                    {        
                        Size prefSizeBefore = pThis->_pCore->calcPreferredSize();

                        UiMargin additionalPadding(UiLength::sem, 1, 2, 3, 4);
                        UiMargin increasedPadding = UiMargin(
                            UiLength::sem,
                            paddingBefore.top.value + additionalPadding.top.value,
                            paddingBefore.right.value + additionalPadding.right.value,
                            paddingBefore.bottom.value + additionalPadding.bottom.value,
                            paddingBefore.left.value + additionalPadding.left.value );

                        // setting padding should increase the preferred size
                        // of the core.
                        pThis->_pView->padding() = increasedPadding;


                        CONTINUE_SECTION_AFTER_PENDING_EVENTS( pThis, prefSizeBefore, additionalPadding )
                        {        
                            // the padding should increase the preferred size.
                            Size prefSize = pThis->_pCore->calcPreferredSize();

                            Margin  additionalPaddingPixels = pThis->_pView->uiMarginToPixelMargin(additionalPadding);

                            REQUIRE_ALMOST_EQUAL( prefSize, prefSizeBefore+additionalPaddingPixels, Size(1,1) );
                        };
                    };
                }
            }
        }

        SECTION("bounds")
        {
            SECTION("manualChange")
            {
                if(canManuallyChangeBounds())
                {
					// note: don't get too big here. If we exceed the screen size then
					// the window size be clipped by the OS.
                    _pView->bounds() = Rect(110, 220, 550, 330);

                    // it may take a layout cycle until the bounds have updated
                    P<TestViewCore> pThis = this;
                    CONTINUE_SECTION_AFTER_PENDING_EVENTS(pThis)
                    {
                        pThis->verifyCoreBounds();
                    };
                }
                else
                {
                    // when the control does not have control over its own bounds then there is sometimes
                    // a delay in the bounds processing.
                    // We must ensure that the control has finished its initial initialization before
                    // we continue. That might take some time in some ports - and a simple
                    // CONTINUE_SECTION_AFTER_PENDING_EVENTS is not enough on all platforms (e.g. winuwp).
                    // So we use CONTINUE_SECTION_AFTER_SECONDS instead
                    P<TestViewCore> pThis = this;

                    CONTINUE_SECTION_AFTER_SECONDS( 2, pThis )
                    {
                        // the control cannot manually change its bounds.
                        // In that case the core must reset the bounds property back
                        // to what it was originally. This reset may be done in a scheduled async call,
                        // so we must process pending events before we test for it.
                        Rect origBounds = pThis->_pView->bounds();

                        // sanity check: at this point the core bounds should always match
                        pThis->verifyCoreBounds();

                        pThis->_pView->bounds() = Rect(117, 227, 887, 997);

                        // again, we must wait until the changes have propagated
                        CONTINUE_SECTION_AFTER_SECONDS(2, pThis, origBounds )
                        {
                            REQUIRE( pThis->_pView->bounds().get() == origBounds );

                            pThis->verifyCoreBounds();
                        };
                    };
                }
            }

            if(coreCanCalculatePreferredSize())
            {
                SECTION("noEffectOnPreferredSize")
                {
                    Size prefSizeBefore = _pCore->calcPreferredSize();

                    _pView->bounds() = Rect(100, 200, 300, 400);
                    
                    REQUIRE( _pCore->calcPreferredSize() == prefSizeBefore );

                    _pView->bounds() = Rect(1000, 2000, 3000, 4000);
                    
                    REQUIRE( _pCore->calcPreferredSize() == prefSizeBefore );
                }
            }
        }
    }


    /** Causes the core object to be created. This is done by adding the view as
        a child to a visible view container or window.*/
    virtual void initCore()
    {
        if(_pView!=cast<View>(_pWindow))
            _pWindow->setContentView( _pView );

        _pCore = _pView->getViewCore();

        REQUIRE( _pCore!=nullptr );
    }

    /** Verifies that the core's visible property matches that of the outer view.*/
    virtual void verifyCoreVisibility()=0;

    /** Verifies that the core's padding property matches that of the outer view.*/
    virtual void verifyCorePadding()=0;


    /** Verifies that the core's bounds property has the initial dummy value used
        directly after initialization.*/
    virtual void verifyInitialDummyCoreBounds()=0;


    /** Verifies that the core's bounds property matches that of the outer view.*/
    virtual void verifyCoreBounds()=0;


    /** Returns the UiProvider to use.*/
    virtual IUiProvider& getUiProvider()=0;


    /** Creates the view object to use for the tests.*/
    virtual P<View> createView()=0;

    /** Sets the view object to use for the tests.*/
    virtual void setView(View* pView)
    {
        _pView = pView;
    }


    /** Returns true if the view core can calculate its preferred size.
        Some core types depend on the outer view to calculate the preferred size
        instead.
        
        The default implementation returns true.
        */
    virtual bool coreCanCalculatePreferredSize()
    {
        return true;
    }

    
    P<Window> _pWindow;
    P<View>   _pView;

    P<IViewCore> _pCore;
};



}
}

#endif

