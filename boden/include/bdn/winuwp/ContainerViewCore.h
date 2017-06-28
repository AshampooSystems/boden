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
            ref new UwpPanelWithCustomLayout( newObj<LayoutDelegate>(pOuter) ),
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

        ::Windows::Foundation::Size measureOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, ::Windows::Foundation::Size winAvailableSize ) override
        {
            // XXX
            OutputDebugString( String( "Container-LayoutDelegate.measureOverride("+std::to_string(winAvailableSize.Width)+", "+std::to_string(winAvailableSize.Height)+"\n" ).asWidePtr() );

            P<ContainerView> pView = _viewWeak.toStrong();

            ::Windows::Foundation::Size winResultSize(0,0);

            if(pView!=nullptr)
            {
                // tell all child views to validate their preferredSize property. That ensures that
                // our calcPreferredSize gets up-to-date results.
                std::list< P<View> > childViews;
                pView->getChildViews( childViews );
                for( auto& pChildView: childViews)
                    pChildView->_doUpdateSizingInfo();

                // forward this to the outer view.
                Size availableSpace = Size::none();

                if( std::isfinite(winAvailableSize.Width) )
                    availableSpace.width = winAvailableSize.Width;

                if( std::isfinite(winAvailableSize.Height) )
                    availableSpace.height = winAvailableSize.Height;

                Size resultSize = pView->calcPreferredSize( availableSpace );



                winResultSize = sizeToUwpSize(resultSize);
            }

            // XXX
            OutputDebugString( String( "/Container-LayoutDelegate.measureOverride.measureOverride()\n" ).asWidePtr() );

            return winResultSize;
        }
        
        ::Windows::Foundation::Size arrangeOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, ::Windows::Foundation::Size winFinalSize ) override
        {
            // XXX
            OutputDebugString( String( "Container-LayoutDelegate.arrangeOverride("+std::to_string(winFinalSize.Width)+", "+std::to_string(winFinalSize.Height)+"\n" ).asWidePtr() );

            P<ContainerView> pView = _viewWeak.toStrong();

            if(pView!=nullptr)
            {
                P<ContainerViewCore> pCore = tryCast<ContainerViewCore>( pView->getViewCore() );

                if(pCore!=nullptr)
                {
                    // forward this to the outer view.
                    pView->_doLayout();
                }
            }            

            // XXX
            OutputDebugString( String( "/Container-LayoutDelegate()\n" ).asWidePtr() );

            return winFinalSize;
        }

    protected:
        WeakP<ContainerView> _viewWeak;
    };
    friend class LayoutDelegate;

    
	UwpPanelWithCustomLayout^ _pUwpView;
	
};

}
}

#endif
