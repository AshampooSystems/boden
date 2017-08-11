#ifndef BDN_TEST_TestWinuwpViewCoreMixin_H_
#define BDN_TEST_TestWinuwpViewCoreMixin_H_


#include <bdn/test/TestViewCore.h>

#include <bdn/View.h>
#include <bdn/winuwp/IUwpViewCore.h>

namespace bdn
{
namespace test
{
    

/** A mixin class that adds implementations of winuwp view specific functionality on top of
    the base class specified in the template parameter BaseClass.*/
template<class BaseClass>
class TestWinuwpViewCoreMixin : public BaseClass
{

protected:

    void initCore() override
    {
        BaseClass::initCore();

        P<bdn::winuwp::IUwpViewCore> pOwner = cast<bdn::winuwp::IUwpViewCore>( _pView->getViewCore() );
        REQUIRE( pOwner!=nullptr );

        _pWinFrameworkElement = pOwner->getFrameworkElement();
        REQUIRE( _pWinFrameworkElement!=nullptr );
    }

    IUiProvider& getUiProvider() override
    {
        return bdn::winuwp::UiProvider::get();
    }

    void verifyCoreVisibility() override
    {
        bool expectedVisible = _pView->visible();
        
        REQUIRE( _pWinFrameworkElement->Visibility == (expectedVisible ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed) );
    }

    virtual UiMargin getExpectedDefaultPadding()
    {
        return UiMargin();
    }

    
    void verifyIsExpectedWinPadding(::Windows::UI::Xaml::Thickness pad)
    {
        UiMargin expectedPadding;
        if(_pView->padding().get().isNull())
            expectedPadding = getExpectedDefaultPadding();
        else
            expectedPadding = _pView->padding().get();

        Margin expectedPixelPadding = _pView->uiMarginToDipMargin( expectedPadding );
        
        REQUIRE_ALMOST_EQUAL( pad.Top, expectedPixelPadding.top, 0.0001);
        REQUIRE_ALMOST_EQUAL( pad.Right, expectedPixelPadding.right, 0.0001);
        REQUIRE_ALMOST_EQUAL( pad.Bottom, expectedPixelPadding.bottom, 0.0001);
        REQUIRE_ALMOST_EQUAL( pad.Left, expectedPixelPadding.left, 0.0001);
    }


    void verifyInitialDummyCoreSize() override
    {        
        double width = _pWinFrameworkElement->Width;
        if(std::isnan(width))
            width = 0;
        double height = _pWinFrameworkElement->Height;
        if(std::isnan(height))
            height = 0;


        // the bounds should NEVER be initialized upon construction.
        // Instead they must be updated by a layout cycle after construction.
        // So at this point in time the bounds should be zero
            
        REQUIRE( width == 0 );
        REQUIRE( height == 0 );
    }

    void verifyCorePosition() override
    {   
        // the position of the framework element is not reflected by any readable
        // frameworkelement properties. The Arrange implementation of the container
        // simply sets it to the value from the View::position() property, but this
        // cannot be verified later.

        // so we do nothing here.
    }


    void verifyCoreSize() override
    {
        // ActualWidth and ActualHeight are apparently not updated until the element
        // is visible. So we cannot use them to verify the size.
        // So, all in all, we cannot do any size verification at all.
        /*
        double width = _pWinFrameworkElement->ActualWidth;
        if(std::isnan(width))
            width = 0;
        double height = _pWinFrameworkElement->ActualHeight;
        if(std::isnan(height))
            height = 0;

        Size size = _pView->size();
        
        REQUIRE_ALMOST_EQUAL( width, size.width, 0.0001 );
        REQUIRE_ALMOST_EQUAL( height, size.height, 0.0001 );*/
    }


    ::Windows::UI::Xaml::FrameworkElement^ _pWinFrameworkElement;
};


}
}


#endif

