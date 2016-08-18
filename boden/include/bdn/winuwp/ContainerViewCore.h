#ifndef BDN_WINUWP_ContainerViewCore_H_
#define BDN_WINUWP_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/winuwp/ViewCore.h>
#include <bdn/winuwp/IParentViewCore.h>

namespace bdn
{
namespace winuwp
{

class ContainerViewCore : public ViewCore, BDN_IMPLEMENTS IParentViewCore
{
private:
	static ::Windows::UI::Xaml::Controls::Canvas^ _createCanvas(ContainerView* pOuter)
	{
		return ref new ::Windows::UI::Xaml::Controls::Canvas();		
	}

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ViewCore(pOuter, _createCanvas(pOuter), ref new ViewCoreEventForwarder(this) )
	{
		_pCanvas = dynamic_cast< ::Windows::UI::Xaml::Controls::Canvas^ >( getFrameworkElement() );
	}


	void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement ) override
	{
		_pCanvas->Children->Append( pUiElement );		
	}


	void setPadding(const Nullable<UiMargin>& uiPadding) override
	{
		// the outer class automatically handles our own padding. So nothing to do here.
	}


		
	Size calcPreferredSize() const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

	int calcPreferredHeightForWidth(int width) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredHeightForWidth must be overloaded in derived class.");
	}

	int calcPreferredWidthForHeight(int height) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredWidthForHeight must be overloaded in derived class.");
	}


protected:
	::Windows::UI::Xaml::Controls::Canvas^ _pCanvas;
	
};

}
}

#endif
