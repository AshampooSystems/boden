#ifndef BDN_TEST_TestGtkViewCoreMixin_H_
#define BDN_TEST_TestGtkViewCoreMixin_H_


#include <bdn/View.h>
#include <bdn/gtk/ViewCore.h>
#include <bdn/gtk/UiProvider.h>

namespace bdn
{
namespace test
{

    

/** A mixin class that adds implementations of GTK view specific functionality on top of
    the base class specified in the template parameter BaseClass.*/
template<class BaseClass>
class TestGtkViewCoreMixin : public BaseClass
{

protected:

    void initCore() override
    {
        BaseClass::initCore();

        _pGtkCore = cast<bdn::gtk::ViewCore>( BaseClass::_pView->getViewCore() );
        REQUIRE( _pGtkCore!=nullptr );

        _pGtkWidget = _pGtkCore->getGtkWidget();
        REQUIRE( _pGtkWidget!=nullptr );
    }

    IUiProvider& getUiProvider() override
    {
        return bdn::gtk::UiProvider::get();
    }

    void verifyCoreVisibility() override
    {
        bool expectedVisible = BaseClass::_pView->visible();
        
        bool visible = gtk_widget_get_visible(_pGtkWidget);
        
        REQUIRE( visible == expectedVisible );
    }
    
    Size getWidgetSize() const
    {
        gint width;
        gint height;                        
            
        if( GTK_IS_WINDOW(_pGtkWidget) )
            gtk_window_get_size( GTK_WINDOW(_pGtkWidget), &width, &height);
        else            
            gtk_widget_get_size_request( _pGtkWidget, &width, &height);
            
        if(width==-1)
            width = 0;
        if(height==-1)
            height = 0;
            
        return Size(width, height);        
    }
    
    Point getWindowPosition() const
    {
        gint x;
        gint y;
        gtk_window_get_position( GTK_WINDOW(_pGtkWidget), &x, &y );        
        
        return Point(x,y);
    }



    void verifyInitialDummyCoreSize() override
    {        
        REQUIRE( getWidgetSize() == Size() );
    }


    void verifyCorePosition() override
    {        
        if(GTK_IS_WINDOW(_pGtkWidget))
            REQUIRE( getWindowPosition() == BaseClass::_pView->position() );        
    }


    void verifyCoreSize() override
    {        
        REQUIRE( getWidgetSize() == BaseClass::_pView->size() );        
    }


    void verifyCorePadding() override
    {
        // the padding is not reflected in GTK widget properties.
        // So nothing to test here.
    }


    P<bdn::gtk::ViewCore> _pGtkCore;
    GtkWidget*            _pGtkWidget;
};



}
}


#endif

