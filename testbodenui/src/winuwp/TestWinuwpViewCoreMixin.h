#ifndef BDN_TEST_TestWinuwpViewCoreMixin_H_
#define BDN_TEST_TestWinuwpViewCoreMixin_H_


#include <bdn/test/TestViewCore.h>

#include <bdn/View.h>
#include <bdn/winuwp/IFrameworkElementOwner.h>

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

        P<bdn::winuwp::IFrameworkElementOwner> pOwner = cast<bdn::winuwp::IFrameworkElementOwner>( _pView->getViewCore() );
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

        Margin expectedPixelPadding = _pView->uiMarginToPixelMargin( expectedPadding );

        double scaleFactor = bdn::winuwp::UiProvider::get().getUiScaleFactor();

        REQUIRE_ALMOST_EQUAL( pad.Top, expectedPixelPadding.top/scaleFactor, 1);
        REQUIRE_ALMOST_EQUAL( pad.Right, expectedPixelPadding.right/scaleFactor, 1);
        REQUIRE_ALMOST_EQUAL( pad.Bottom, expectedPixelPadding.bottom/scaleFactor, 1);
        REQUIRE_ALMOST_EQUAL( pad.Left, expectedPixelPadding.left/scaleFactor, 1);
    }


    void verifyInitialDummyCoreBounds() override
    {        
        double x = ::Windows::UI::Xaml::Controls::Canvas::GetLeft(_pWinFrameworkElement);
        double y = ::Windows::UI::Xaml::Controls::Canvas::GetTop(_pWinFrameworkElement);
        double width = _pWinFrameworkElement->Width;
        if(std::isnan(width))
            width = 0;
        double height = _pWinFrameworkElement->Height;
        if(std::isnan(height))
            height = 0;


        // the bounds should NEVER be initialized upon construction.
        // Instead they must be updated by a layout cycle after construction.
        // So at this point in time the bounds should be zero
            
        REQUIRE( x == 0 );
        REQUIRE( y == 0 );
        REQUIRE( width == 0 );
        REQUIRE( height == 0 );
    }

    void verifyCoreBounds() override
    {        
        double x = ::Windows::UI::Xaml::Controls::Canvas::GetLeft(_pWinFrameworkElement);
        double y = ::Windows::UI::Xaml::Controls::Canvas::GetTop(_pWinFrameworkElement);
        double width = _pWinFrameworkElement->Width;
        if(std::isnan(width))
            width = 0;
        double height = _pWinFrameworkElement->Height;
        if(std::isnan(height))
            height = 0;

        Rect bounds = _pView->bounds();

        double scaleFactor = bdn::winuwp::UiProvider::get().getUiScaleFactor();

        REQUIRE_ALMOST_EQUAL( x, bounds.x/scaleFactor, 1 );
        REQUIRE_ALMOST_EQUAL( y, bounds.y/scaleFactor, 1 );
        REQUIRE_ALMOST_EQUAL( width, bounds.width/scaleFactor, 1 );
        REQUIRE_ALMOST_EQUAL( height, bounds.height/scaleFactor, 1 );
    }


    ::Windows::UI::Xaml::FrameworkElement^ _pWinFrameworkElement;
};


}
}


#endif

