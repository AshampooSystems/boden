#ifndef BDN_WINUWP_ContainerViewCore_H_
#define BDN_WINUWP_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/winuwp/ChildViewCore.h>
#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/UwpPanelWithCustomLayout.h>

namespace bdn
{
namespace winuwp
{

class ContainerViewCore : public ChildViewCore, BDN_IMPLEMENTS IViewCoreParent
{

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ChildViewCore(
            pOuter,
            UwpPanelWithCustomLayout::createInstance( newObj<LayoutDelegate>(pOuter) ),
            ref new ViewCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		_pUwpView = dynamic_cast< UwpPanelWithCustomLayout^ >( getFrameworkElement() );
        
        BDN_WINUWP_TO_STDEXC_END;
	}


	void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement ) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pUwpView->Children->Append( pUiElement );		

        BDN_WINUWP_TO_STDEXC_END;        
	}


	void setPadding(const Nullable<UiMargin>& uiPadding) override
	{
		// the outer class automatically handles our own padding. So nothing to do here.
	}
    		
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

private:

    class LayoutDelegate : public Base, BDN_IMPLEMENTS IUwpLayoutDelegate
    {
    public:
        LayoutDelegate(ContainerView* pView)
            : _viewWeak(pView)
        {
        }

        ::Windows::Foundation::Size measureOverride( ::Windows::Foundation::Size winAvailableSize ) override
        {
            P<ContainerView> pView = _viewWeak.toStrong();

            if(pView!=nullptr)
            {
               // forward this to the outer view.
              Size availableSpace = Size::none();

                if( std::isfinite(winAvailableSize.Width) )
                    availableSpace.width = winAvailableSize.Width;

                if( std::isfinite(winAvailableSize.Height) )
                    availableSpace.height = winAvailableSize.Height;

                Size resultSize = pView->calcPreferredSize( availableSpace );

                return sizeToUwpSize(resultSize);
            }
            else
                return ::Windows::Foundation::Size(0, 0);
        }
        
        ::Windows::Foundation::Size arrangeOverride( ::Windows::Foundation::Size finalSize ) override
        {
            P<ContainerView> pView = _viewWeak.toStrong();

            if(pView!=nullptr)
            {
                // forward this to the outer view.
                pView->_doLayout();
            }            

            return finalSize;
        }

    protected:
        WeakP<ContainerView> _viewWeak;

    };

    
	UwpPanelWithCustomLayout^ _pUwpView;
	
};

}
}

#endif
