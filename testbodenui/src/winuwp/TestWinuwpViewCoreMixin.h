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

        Margin expectedPixelPadding = _pView->uiMarginToDipMargin( expectedPadding );
        
        REQUIRE_ALMOST_EQUAL( pad.Top, expectedPixelPadding.top, 0.0001);
        REQUIRE_ALMOST_EQUAL( pad.Right, expectedPixelPadding.right, 0.0001);
        REQUIRE_ALMOST_EQUAL( pad.Bottom, expectedPixelPadding.bottom, 0.0001);
        REQUIRE_ALMOST_EQUAL( pad.Left, expectedPixelPadding.left, 0.0001);
    }


    void verifyInitialDummyCorePosition() override
    {        
        double x = ::Windows::UI::Xaml::Controls::Canvas::GetLeft(_pWinFrameworkElement);
        double y = ::Windows::UI::Xaml::Controls::Canvas::GetTop(_pWinFrameworkElement);

        // the bounds should NEVER be initialized upon construction.
        // Instead they must be updated by a layout cycle after construction.
        // So at this point in time the bounds should be zero
            
        REQUIRE( x == 0 );
        REQUIRE( y == 0 );
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
        // ensure that pending changes are reflected in the layout
        _pWinFrameworkElement->UpdateLayout();

        double x = ::Windows::UI::Xaml::Controls::Canvas::GetLeft(_pWinFrameworkElement);
        double y = ::Windows::UI::Xaml::Controls::Canvas::GetTop(_pWinFrameworkElement);
        
        Point pos = _pView->position();
        
        REQUIRE_ALMOST_EQUAL( x, pos.x, 0.0001 );
        REQUIRE_ALMOST_EQUAL( y, pos.y, 0.0001 );
    }


    void verifyCoreSize() override
    {        
        double width = _pWinFrameworkElement->Width;
        if(std::isnan(width))
            width = 0;
        double height = _pWinFrameworkElement->Height;
        if(std::isnan(height))
            height = 0;

        Size size = _pView->size();
        
        REQUIRE_ALMOST_EQUAL( width, size.width, 0.0001 );
        REQUIRE_ALMOST_EQUAL( height, size.height, 0.0001 );
    }


    ::Windows::UI::Xaml::FrameworkElement^ _pWinFrameworkElement;
};


}
}


#endif

