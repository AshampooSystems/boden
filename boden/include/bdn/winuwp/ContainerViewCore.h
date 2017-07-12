#ifndef BDN_WINUWP_ContainerViewCore_H_
#define BDN_WINUWP_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/winuwp/ChildViewCore.h>
#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/UwpViewWithLayoutDelegate.h>

namespace bdn
{
namespace winuwp
{

class ContainerViewCore :
    public ChildViewCore,
    BDN_IMPLEMENTS IUwpLayoutDelegate,
    BDN_IMPLEMENTS IViewCoreParent
{

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ChildViewCore(
            pOuter,
            ref new UwpViewWithLayoutDelegate<>(),
            ref new ViewCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// note that 
		_pUwpView = dynamic_cast< UwpViewWithLayoutDelegate<>^ >( getFrameworkElement() );

		// it is ok to set this as the delegate, since the panel only holds a weak reference
		// to the delegate. I.e. there will not be a reference cycle.
		_pUwpView->setLayoutDelegateWeak(this);
        
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

        
	void layout() override
	{
        P<ContainerView> pOuterView = cast<ContainerView>( getOuterViewIfStillAttached() );
        if(pOuterView!=nullptr)
        {        
            P<ViewLayout> pLayout = pOuterView->calcContainerLayout( pOuterView->size() );
            pLayout->applyTo(pOuterView);
        }
	}

	
    Size uwpMeasureOverride(const Size& availableSpace ) override
	{
        OutputDebugString( String( "ContainerViewCore.measureOverride("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+")\n" ).asWidePtr() );
            
		P<View> pOuterView = getOuterViewIfStillAttached();

        Size resultSize;
		if(pOuterView!=nullptr)
			resultSize = pOuterView->calcPreferredSize(availableSpace);

        OutputDebugString( String( "/ContainerViewCore.measureOverride -> "+std::to_string(resultSize.width)+", "+std::to_string(resultSize.height)+"\n" ).asWidePtr() );            

        return resultSize;
	}

    void finalizeUwpMeasure(const Size& lastMeasureAvailableSpace) override
    {
        P<View> pOuterView = getOuterViewIfStillAttached();

		if(pOuterView!=nullptr)
            defaultFinalizeUwpMeasure(pOuterView, lastMeasureAvailableSpace);
    }


    /** Implementation for the ArrangeOverride UWP function.*/
    Size uwpArrangeOverride(const Size& finalSize ) override
	{
		P<View> pOuterView = getOuterViewIfStillAttached();

        if(pOuterView!=nullptr)
            return defaultArrangeOverride(pOuterView, finalSize);		
        else
            return finalSize;
	}
	
private:
	    
	UwpViewWithLayoutDelegate<>^ _pUwpView;
};

}
}

#endif
