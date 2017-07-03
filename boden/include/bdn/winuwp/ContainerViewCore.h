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

			_cachedLayouts.clear();

            if(pView!=nullptr)
            {
                // tell all child views to validate their preferredSize property. That ensures that
                // our calcPreferredSize gets up-to-date results.
                std::list< P<View> > childViews;
                pView->getChildViews( childViews );
                for( auto& pChildView: childViews)
                    pChildView->_doUpdateSizingInfo();

				Size availableSpace = Size::none();

				if( std::isfinite(winAvailableSize.Width) )
					availableSpace.width = winAvailableSize.Width;

				if( std::isfinite(winAvailableSize.Height) )
					availableSpace.height = winAvailableSize.Height;

				// first calculate our preferred size for the specified amount of available space.
				Size preferredSize = pView->calcPreferredSize(availableSpace); 

				// we cannot calculate layouts during Arrange, since calling calcPreferredSize/Measure
				// on child views should not be done in Arrange (it can cause layout cycles).
				// So we calculate the layout in measure and store it. Then in arrange we simply apply
				// the layout.
				// We calc the layout for the case that we end up with our preferred size
				P<ViewLayout> pPreferredSizeLayout = pView->calcLayout(preferredSize);
				
				_cachedLayouts.push_back( CachedLayout{preferredSize, pPreferredSizeLayout} );

				if( std::isfinite(availableSpace.width) && std::isfinite(availableSpace.height) )
				{
					// we also calculate the layout for the case that we end up covering the whole
					// availableSpace.
					P<ViewLayout> pFullLayout = pView->calcLayout(availableSpace);
					_cachedLayouts.push_back( CachedLayout{availableSpace, pFullLayout} );
				}
				
				// If we do not end up with any of the two sizes above then we willset _additionalLayoutSize
				// in arrangeOverride and schedule a re-measure. So if this is set then we also need to cache
				// the layout for that.
				if(_additionalLayoutSizeEnabled)
				{
					P<ViewLayout> pAdditionalLayout = pView->calcLayout( _additionalLayoutSize );
					_cachedLayouts.push_back( CachedLayout{_additionalLayoutSize, pAdditionalLayout} );
				}

				// IMPORTANT: Windows Bug Note (as of Windows 10 from 2017-07-03)
				// In contrast to the documentation, Panel objects apparently cannot
				// be made smaller than their DesiredSize. Their Arrange method will always
				// enlarge the size we pass to it and make the panel at least as big as the desired size.
				// Since we absolutely do not want that we need to ensure that the DesiredSize is (0,0).
				// That does not interfere with our own layout, since we do not use DesiredSize to
				// size this window panel - instead we always make it the same size as the window.
				winResultSize.Width = 0;
				winResultSize.Height = 0;
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
				Size containerSize = uwpSizeToSize(winFinalSize);

				// if the container size does not match the additional layout size
				// anymore then we disable the additional measuring.
				if(_additionalLayoutSizeEnabled && containerSize!=_additionalLayoutSize)
					_additionalLayoutSizeEnabled = false;
				
				P<ViewLayout> pSelectedLayout;

				for( auto& cachedLayout: _cachedLayouts)
				{
					Size sizeDiff = cachedLayout.forSize - containerSize;

					// if the container size is close to the size of the cached layout then we use that
					if( fabs(sizeDiff.width)<0.1 && fabs(sizeDiff.height)<0.1 )
					{
						// matches
						pSelectedLayout = cachedLayout.pLayout;
						break;
					}
				}

				if(pSelectedLayout==nullptr)
				{
					// none of the cached layouts match. So we HAVE to calculate a new layout for the current
					// size.

					_additionalLayoutSizeEnabled = true;
					_additionalLayoutSize = containerSize;

					// schedule a remeasure asynchronously. That will cause windows to call our measureOverride and
					// then rearrange us.
					
					// We can either call calcLayout directly or we can call InvalidateMeasure.
					// It is probably safer to schedule this asynchronously instead of calling calcLayout directly.
					// The re-measuring of the child views that happens in calcLayout will trigger another
					// Arrange of ourselves and it is uncertain how windows will handle it if that happens
					// while Arrange is executing. Better not risk it.

					pPanel->InvalidateMeasure();
				}
				else
				{
					// apply the layout
					pSelectedLayout->applyTo( pView );
				}
            }

            // XXX
            OutputDebugString( String( "/Container-LayoutDelegate()\n" ).asWidePtr() );

            return winFinalSize;
        }

    protected:
        WeakP<ContainerView> _viewWeak;

		struct CachedLayout
		{
			Size			forSize;
			P<ViewLayout>	pLayout;
		};

		std::list< CachedLayout >  _cachedLayouts;

		bool					   _additionalLayoutSizeEnabled = false;
		Size					   _additionalLayoutSize;
    };
    friend class LayoutDelegate;
	    
	UwpPanelWithCustomLayout^ _pUwpView;
	
};

}
}

#endif
