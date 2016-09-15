#ifndef BDN_WINUWP_ContainerViewCore_H_
#define BDN_WINUWP_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/winuwp/ChildViewCore.h>
#include <bdn/winuwp/IViewCoreParent.h>

namespace bdn
{
namespace winuwp
{

class ContainerViewCore : public ChildViewCore, BDN_IMPLEMENTS IViewCoreParent
{
private:
	static ::Windows::UI::Xaml::Controls::Canvas^ _createCanvas(ContainerView* pOuter)
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		return ref new ::Windows::UI::Xaml::Controls::Canvas();		

        BDN_WINUWP_TO_STDEXC_END;
	}

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ChildViewCore(pOuter, _createCanvas(pOuter), ref new ViewCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		_pCanvas = dynamic_cast< ::Windows::UI::Xaml::Controls::Canvas^ >( getFrameworkElement() );

        BDN_WINUWP_TO_STDEXC_END;
	}


	void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement ) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pCanvas->Children->Append( pUiElement );		

        BDN_WINUWP_TO_STDEXC_END;        
	}


	void setPadding(const Nullable<UiMargin>& uiPadding) override
	{
		// the outer class automatically handles our own padding. So nothing to do here.
	}


		
	Size calcPreferredSize(int availableWidth=-1, int availableHeight=-1) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

protected:
    
	::Windows::UI::Xaml::Controls::Canvas^ _pCanvas;
	
};

}
}

#endif
