#ifndef BDN_TEST_TestAndroidViewCore_H_
#define BDN_TEST_TestAndroidViewCore_H_


#include <bdn/View.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/UiProvider.h>

namespace bdn
{
namespace test
{


/** A mixin class that adds implementations of android view specific functionality on top of
 the base class specified in the template parameter BaseClass.*/
template<class BaseClass>
class TestAndroidViewCoreMixin : public BaseClass
{
protected:
    
    void initCore() override
    {
        BaseClass::initCore();
        
        _pAndroidViewCore = cast<bdn::android::ViewCore>( BaseClass::_pView->getViewCore() );
        REQUIRE( _pAndroidViewCore!=nullptr );
        
        _jView = _pAndroidViewCore->getJView();
        REQUIRE( !_jView.isNull_() );
    }
    
    IUiProvider& getUiProvider() override
    {
        return bdn::android::UiProvider::get();
    }
    
    void verifyCoreVisibility() override
    {
        bool expectedVisible = BaseClass::_pView->visible();

        if(expectedVisible)
            REQUIRE( _jView.getVisibility()==bdn::android::JView::Visibility::visible );
        else
            REQUIRE( _jView.getVisibility()==bdn::android::JView::Visibility::invisible );
    }
    
    Rect getViewRect()
    {
        double scaleFactor = _pAndroidViewCore->getUiScaleFactor();

        return Rect( _jView.getLeft()/scaleFactor, _jView.getTop()/scaleFactor, _jView.getWidth()/scaleFactor, _jView.getHeight()/scaleFactor );
    }
    
    void verifyInitialDummyCoreBounds() override
    {        
        Rect rect = getViewRect();

        REQUIRE( rect == Rect() );
    }
    
    void verifyCoreBounds() override
    {        
        Rect rect = getViewRect();
        Rect expectedRect = BaseClass::_pView->bounds();
        
        REQUIRE( rect == expectedRect );
    }
    
    
    void verifyCorePadding() override
    {
        int top = _jView.getPaddingTop();
        int right = _jView.getPaddingRight();
        int bottom = _jView.getPaddingBottom();
        int left = _jView.getPaddingLeft();

        Nullable<UiMargin> pad = BaseClass::_pView->padding().get();

        Margin expectedPadding;
        if(pad.isNull())
        {
            // the default padding is used. This is simply the padding that is pre-configured
            // in the android UI control.
            // We have no way to test if what we get here is really correct. So we cannot test this case.
            return;
        }

        double scaleFactor = _pAndroidViewCore->getUiScaleFactor();

        expectedPadding = BaseClass::_pView->uiMarginToDipMargin( pad.get() );

        REQUIRE( top/scaleFactor == expectedPadding.top );
        REQUIRE( right/scaleFactor ==expectedPadding.right );
        REQUIRE( bottom/scaleFactor ==expectedPadding.bottom );
        REQUIRE( left/scaleFactor ==expectedPadding.left );
    }
    
    
    P<bdn::android::ViewCore>  _pAndroidViewCore;
    bdn::android::JView        _jView;
};

        
        
}
}


#endif

