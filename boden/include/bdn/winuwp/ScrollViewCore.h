#ifndef BDN_WINUWP_ScrollViewCore_H_
#define BDN_WINUWP_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>
#include <bdn/winuwp/ChildViewCore.h>
#include <bdn/ScrollViewLayoutHelper.h>

namespace bdn
{
namespace winuwp
{

class ScrollViewCore : public ChildViewCore,
                        BDN_IMPLEMENTS IScrollViewCore,
                        BDN_IMPLEMENTS IViewCoreParent
{
private:
	static ::Windows::UI::Xaml::Controls::ScrollViewer^ _createScrollViewer(ScrollView* pOuter)
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		::Windows::UI::Xaml::Controls::ScrollViewer^ pScrollViewer = ref new ::Windows::UI::Xaml::Controls::ScrollViewer();		

		return pScrollViewer;

        BDN_WINUWP_TO_STDEXC_END;
	}

public:

	ScrollViewCore(	ScrollView* pOuter)
		: ChildViewCore(pOuter, _createScrollViewer(pOuter), ref new ViewCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;
                
		_pScrollViewer = dynamic_cast< ::Windows::UI::Xaml::Controls::ScrollViewer^ >( getFrameworkElement() );

        // we want to let the scroll viewer handle the scroll bars and layouting automatically.
        // It is compatible with our own layout system.
        // However, we still want to handle the layout ourselves within the viewport (padding, margin
        // of the content view).
        // To achieve that we have to give the scroll viewer has an UWP content panel
        // that wraps the actual content view and forwards layout and measure calls accordingly.
        _pContentWrapper = ref new UwpViewWithLayoutDelegate<>();
        _pContentWrapperLayoutDelegate = newObj<ContentWrapperLayoutDelegate>(pOuter);
        _pContentWrapper->setLayoutDelegateWeak( _pContentWrapperLayoutDelegate );

		_pScrollViewer->Content = _pContentWrapper;

		/*::Windows::UI::Xaml::Controls::ScrollContentPresenter^ pPresenter = ref new ::Windows::UI::Xaml::Controls::ScrollContentPresenter;

		_pScrollViewer->Content = pPresenter;
		pPresenter->ScrollOwner = _pScrollViewer;

		pPresenter->Content = _pContentWrapper;*/
						
        setHorizontalScrollingEnabled( pOuter->horizontalScrollingEnabled() );
        setVerticalScrollingEnabled( pOuter->verticalScrollingEnabled() );
        
        BDN_WINUWP_TO_STDEXC_END;
	}


    

	void setPadding(const Nullable<UiMargin>& pad) override
	{
        // we do not pass the padding on to the scroll view. We control it manually
        // when we lay out the content wrapper.

        // But we must invalidate the measure and arrange info of the
        // content wrapper
        UwpLayoutBridge::get().invalidateMeasure(_pContentWrapper);
        UwpLayoutBridge::get().invalidateArrange(_pContentWrapper);
	}


    
	void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement ) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;
        
        // we have only one child (our own content view).
        _pContentWrapper->Children->Clear();

		_pContentWrapper->Children->Append(pUiElement);

        BDN_WINUWP_TO_STDEXC_END;
	}
	


	void setHorizontalScrollingEnabled(const bool& enabled)
    {
        _pScrollViewer->HorizontalScrollMode = enabled ? ::Windows::UI::Xaml::Controls::ScrollMode::Enabled : ::Windows::UI::Xaml::Controls::ScrollMode::Disabled;
    }

	void setVerticalScrollingEnabled(const bool& enabled)
    {
        _pScrollViewer->VerticalScrollMode = enabled ? ::Windows::UI::Xaml::Controls::ScrollMode::Enabled : ::Windows::UI::Xaml::Controls::ScrollMode::Disabled;
    }
    

    void layout()
    {
        // Windows takes care of the layout of the scroll viewer. But we still need
        // to lay out the content view inside of our content wrapper.

        // layout is called at the end of the measure phase. I.e. at a point in time
        // when the content wrapper does not have its final size yet.
        // BUT the viewport size (i.e. the size of the scrollview) is known, since View::size()
        // is already up-to-date. So we simply use the ScrolViewLayoutHelper here.

        P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );		

        if(pOuter!=nullptr)
        {
            ScrollViewLayoutHelper helper( 0, 0);

            helper.calcLayout(pOuter, pOuter->size() );

            Rect contentBounds = helper.getContentViewBounds();

            P<View> pContentView = pOuter->getContentView();
            if(pContentView!=nullptr)
                pContentView->adjustAndSetBounds(contentBounds);
        }
    }


    

    void invalidateSizingInfo( View::InvalidateReason reason ) override
    {
        ChildViewCore::invalidateSizingInfo(reason );

        if(reason!=View::InvalidateReason::standardPropertyChanged && reason!=View::InvalidateReason::standardChildPropertyChanged )
        {
            // also invalidate the measurements of the content wrapper
            UwpLayoutBridge::get().invalidateMeasure( _pContentWrapper );
        }
    }


    
    void needLayout(View::InvalidateReason reason) override
    {
        ChildViewCore::needLayout(reason);

        if(reason!=View::InvalidateReason::standardPropertyChanged && reason!=View::InvalidateReason::standardChildPropertyChanged )
        {
            // also invalidate the layout of the content wrapper.
            // The default implementation (called above) invalidates the scroll viewer itself.
            UwpLayoutBridge::get().invalidateMeasure( _pContentWrapper );
            UwpLayoutBridge::get().invalidateArrange( _pContentWrapper );
        }
    }

    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		// Note that we cannot call ChildViewCore::calcPreferredSize here to get
		// our desired size, since that uses Measure.
		// And for us Measure will always return 0. That is by design, since Windows does
		// not allow controls to be sized smaller than their DesiredSize. So to allow us
		// to have full control over the size we force the DesiredSize to be zero.

		// So we have to calculate our preferred size ourselves by looking at the
		// content view. We can use the layout helper for that.
				
		// note that we still have to call Measure on the control to ensure that
		// DesiredSize is initialized and to ensure that it participates in the layout
		// (Windows only includes controls that have Measure called on them).
		// And we have to do that first, since the scroll viewer's measure method calls
		// Measure on the content view and will thus initialize the content view's desired size. And we have to make
		// sure that we overwrite that afterwards.
		::Windows::Foundation::Size winAvailSpace = sizeToUwpSize(availableSpace);
            
		_pScrollViewer->Measure( winAvailSpace );

        // calling Measure on the scroll viewer will automatically call measure on the content wrapper.
        
        P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );		

		Size prefSize;
        if(pOuter!=nullptr)
        {
			// scroll bars are overlays that only appear when they are used.
			// So they do not take up any space.		
			ScrollViewLayoutHelper layoutHelper(0,0);
            
			prefSize = layoutHelper.calcPreferredSize(pOuter, availableSpace);
		}

		return prefSize;
    }

protected:
    virtual bool canAdjustWidthToAvailableSpace() const
	{
		return true;
	}

    virtual bool canAdjustHeightToAvailableSpace() const
	{
		return true;
	}


    void _uwpSizeChanged() override
	{
        // XXX
        OutputDebugString( L"ScrollViewCore._uwpSizeChanged()\n" );

        ChildViewCore::_uwpSizeChanged();

        // the scroll viewer's size has changed. Ensure that we update the DesiredSize that the content
        // reports, because the desired size may depend on the size and/or shape of the viewport.

        UwpLayoutBridge::get().invalidateMeasure( _pContentWrapper );
	}

private:
    
    class ContentWrapperLayoutDelegate : public Base, BDN_IMPLEMENTS IUwpLayoutDelegate
    {
    public:
        ContentWrapperLayoutDelegate( ScrollView* pView)
            : _viewWeak(pView)
        {
        }

        Size uwpMeasureOverride(const Size& availableSpace ) override
        {
            // XXX
            OutputDebugString( String( "ContentWrapperLayoutDelegate.measureOverride("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+"\n" ).asWidePtr() );

            P<ScrollView> pView = _viewWeak.toStrong();

            Size resultSize;
            if(pView!=nullptr)
            {
                // we cannot use the scrollview layouthelper here because we need to calculate the
                // desired size of the content wrapper, not the entire scrollview.

                P<View> pContentView = pView->getContentView();
                if(pContentView!=nullptr)
                {
                    Margin padding;                    
                    Nullable<UiMargin> pad = pView->padding();
                    if(!pad.isNull())
                        padding = pView->uiMarginToDipMargin(pad);            

                    Margin contentMargin = pContentView->uiMarginToDipMargin( pContentView->margin() );

                    Size availableContentSpace = availableSpace;

                    if(pView->horizontalScrollingEnabled() )
                        availableContentSpace.width = Size::componentNone();

                    if(pView->verticalScrollingEnabled() )
                        availableContentSpace.height = Size::componentNone();

                    if(std::isfinite( availableContentSpace.width ) )
                        availableContentSpace.width -= padding.left + padding.right + contentMargin.left + contentMargin.right;

                    if(std::isfinite( availableContentSpace.height ) )
                        availableContentSpace.height -= padding.top + padding.bottom + contentMargin.top + contentMargin.bottom;

                    resultSize = pContentView->calcPreferredSize( availableContentSpace );
                    
                    resultSize += contentMargin + padding;
                }
            }

            OutputDebugString( String( "/ContentWrapperLayoutDelegate.measureOverride\n" ).asWidePtr() );
            
            return resultSize;
        }

        void finalizeUwpMeasure(const Size& lastMeasureAvailableSpace) override
        {
            // we need to do the finalization on the content wrapper.

            // the content wrapper will automatically sized by the ScrollViewer, so we do not
            // need to adjust its own size.
            // But we do need to size the content view and update its layout.

            // our scrollview layout function will take care of that.
            P<ScrollView> pView = _viewWeak.toStrong();            
            if(pView!=nullptr)
                layoutViewTree( pView );
        }
        
        Size uwpArrangeOverride(const Size& finalSize ) override
        {
            // XXX
            OutputDebugString( String( "ContentWrapperLayoutDelegate.arrangeOverride("+std::to_string(finalSize.width)+", "+std::to_string(finalSize.height)+"\n" ).asWidePtr() );

            P<ScrollView> pView = _viewWeak.toStrong();

            if(pView!=nullptr)
            {
                // we simply need to arrange the content view to the rect we calculated earlier.
                P<View> pContentView = pView->getContentView();
                if(pContentView!=nullptr)
                {
                    P<ChildViewCore> pContentCore = tryCast<ChildViewCore>( pContentView->getViewCore() );
                    if(pContentCore!=nullptr)
                    {
                        ::Windows::UI::Xaml::FrameworkElement^ pContentElement = pContentCore->getFrameworkElement();
                        if(pContentElement!=nullptr)
                        {
					        Rect						contentBounds(pContentView->position(), pContentView->size());

                            OutputDebugString( String( "ScrollView content view "+String(pContentElement->GetType()->ToString()->Data())+" arrange: "+std::to_string(contentBounds.x)+", "+std::to_string(contentBounds.y)+", "+std::to_string(contentBounds.width)+", "+std::to_string(contentBounds.height)+"\n" ).asWidePtr() );
                            
					        ::Windows::Foundation::Rect winContentBounds = rectToUwpRect( contentBounds );
                            				
					        pContentElement->Arrange(winContentBounds);

                            // XXX
                            double width = pContentElement->ActualWidth;
                            double height = pContentElement->ActualHeight;

                            OutputDebugString( String( "Resulting size: "+std::to_string(width)+" x "+std::to_string(height)+"\n" ).asWidePtr() );
				        }
                    }
                }
                
                P<ScrollViewCore> pCore = tryCast<ScrollViewCore>( pView->getViewCore() );
				if(pCore!=nullptr)
					pCore->_pScrollViewer->InvalidateScrollInfo();
            }     

            // XXX
            OutputDebugString( String( "/ContentWrapperLayoutDelegate.arrangeOverride()\n" ).asWidePtr() );

            return finalSize;
        }

    protected:
        WeakP<ScrollView> _viewWeak;
    };
    friend class ContentWrapperLayoutDelegate;


    ::Windows::UI::Xaml::Controls::ScrollViewer^ _pScrollViewer;
    UwpViewWithLayoutDelegate<>^                 _pContentWrapper;
    P<ContentWrapperLayoutDelegate>              _pContentWrapperLayoutDelegate;
};

}
}

#endif
