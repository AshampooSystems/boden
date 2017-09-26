#ifndef BDN_WINUWP_ScrollViewCore_H_
#define BDN_WINUWP_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>
#include <bdn/winuwp/ChildViewCore.h>
#include <bdn/ScrollViewLayoutHelper.h>
#include <bdn/winuwp/UwpViewWithLayoutDelegate.h>

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
		: ChildViewCore(pOuter, _createScrollViewer(pOuter), ref new ScrollViewCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;
                
		_pScrollViewer = dynamic_cast< ::Windows::UI::Xaml::Controls::ScrollViewer^ >( getFrameworkElement() );

        _pScrollViewer->HorizontalScrollBarVisibility = ::Windows::UI::Xaml::Controls::ScrollBarVisibility::Auto;
        _pScrollViewer->VerticalScrollBarVisibility = ::Windows::UI::Xaml::Controls::ScrollBarVisibility::Auto;

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


		_pScrollViewer->ViewChanged += ref new ::Windows::Foundation::EventHandler<::Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^>(
            dynamic_cast<ScrollViewCoreEventForwarder^>(getViewCoreEventForwarder()),
            &ScrollViewCoreEventForwarder::viewChanged );


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
	


	void setHorizontalScrollingEnabled(const bool& enabled) override
    {
        _pScrollViewer->HorizontalScrollMode = enabled ? ::Windows::UI::Xaml::Controls::ScrollMode::Enabled : ::Windows::UI::Xaml::Controls::ScrollMode::Disabled;
    }

	void setVerticalScrollingEnabled(const bool& enabled) override
    {
        _pScrollViewer->VerticalScrollMode = enabled ? ::Windows::UI::Xaml::Controls::ScrollMode::Enabled : ::Windows::UI::Xaml::Controls::ScrollMode::Disabled;
    }

    void scrollClientRectToVisible(const Rect& clientRect) override
    {
        Rect visibleRect;
        visibleRect.x = _pScrollViewer->HorizontalOffset;
        visibleRect.y = _pScrollViewer->VerticalOffset;
        visibleRect.width = _pScrollViewer->ViewportWidth;
        visibleRect.height = _pScrollViewer->ViewportHeight;

        Size clientSize;
        clientSize.width = _pContentWrapper->ActualWidth;
        clientSize.height = _pContentWrapper->ActualHeight;

        auto hm = _pScrollViewer->HorizontalScrollMode;
        auto vm = _pScrollViewer->VerticalScrollMode;

        double targetLeft = clientRect.x;
        double targetRight = clientRect.x + clientRect.width;
        double targetTop = clientRect.y;
        double targetBottom = clientRect.y + clientRect.height;
        
        // first, clip the target rect to the client area.
        // This also automatically gets rid of infinity target positions (which are allowed)
        if( targetLeft > clientSize.width)
            targetLeft = clientSize.width;
        if( targetRight > clientSize.width )
            targetRight = clientSize.width;
        if( targetTop > clientSize.height)
            targetTop = clientSize.height;
        if( targetBottom > clientSize.height )
            targetBottom = clientSize.height;

        if(targetLeft<0)
            targetLeft = 0;
        if(targetRight<0)
            targetRight = 0;
        if(targetTop<0)
            targetTop = 0;
        if(targetBottom<0)
            targetBottom = 0;

        
        // there is a special case if the target rect is bigger than the viewport.
        // In that case the desired end position is ambiguous: any sub-rect of viewport size
        // inside the specified target rect would be "as good as possible".
        // The documentation for scrollClientRectToVisible resolves this ambiguity by requiring
        // that we scroll the minimal amount. So we want the new visible rect to be as close
        // to the old one as possible.
    
        // Since we specify the scroll position directly, we need to handle this case on our side.
       
    
        if(targetRight-targetLeft > visibleRect.width)
        {
            // the width of the target rect is bigger than the viewport width.
            double visibleRight = visibleRect.x + visibleRect.width;
        
            if( visibleRect.x >= targetLeft && visibleRight<=targetRight)
            {
                // The current visible rect is already fully inside the target rect.
                // In this case we do not want to move the scroll position at all.
                // So set the target rect to the current view port rect
                targetLeft = visibleRect.x;
                targetRight = visibleRight;
            }
            else
            {
                // shrink the target rect so that it matches the viewport width.
                // We want to shrink towards the edge that is closest to the current visible rect.
                // Note that the width of the visible rect is smaller than the target width and
                // that the visible rect is not fully inside the target rect.
                // So one of the target rect edges has to be closer than the other.
            
                double distanceLeft = fabs( targetLeft-visibleRect.x );
                double distanceRight = fabs( targetRight-visibleRight );
            
                if(distanceLeft<distanceRight)
                {
                    // the left edge of the target rect is closer to the current visible rect
                    // than the right edge. So we want to move towards the left.
                    targetRight = targetLeft + visibleRect.width;
                }
                else
                {
                    // move towards the right edge
                    targetLeft = targetRight - visibleRect.width;
                }
            }
        }
    
        if(targetBottom - targetTop > visibleRect.height)
        {
            double visibleBottom = visibleRect.y+visibleRect.height;

            if( visibleRect.y >= targetTop && visibleBottom<=targetBottom)
            {
                targetTop = visibleRect.y;
                targetBottom = visibleBottom;
            }
            else
            {
                double distanceTop = fabs( targetTop-visibleRect.y );
                double distanceBottom = fabs( targetBottom-visibleBottom );
            
                if(distanceTop<distanceBottom)
                    targetBottom = targetTop + visibleRect.height;
                else
                    targetTop = targetBottom - visibleRect.height;
            }
        }
    
        if(targetLeft<0)
            targetLeft = 0;
        if(targetRight<0)
            targetRight = 0;
        if(targetTop<0)
            targetTop = 0;
        if(targetBottom<0)
            targetBottom = 0;

        if(targetRight > visibleRect.x + visibleRect.width)
            visibleRect.x = targetRight - visibleRect.width;
        if(targetLeft < visibleRect.x)
            visibleRect.x = targetLeft;

        if(targetBottom > visibleRect.y + visibleRect.height)
            visibleRect.y = targetBottom - visibleRect.height;
        if(targetTop < visibleRect.y)
            visibleRect.y = targetTop;
        
        _pScrollViewer->ChangeView( visibleRect.x, visibleRect.y, _pScrollViewer->ZoomFactor );
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
        // OutputDebugString( L"ScrollViewCore._uwpSizeChanged()\n" );

        ChildViewCore::_uwpSizeChanged();

        updateVisibleRect();

        // the scroll viewer's size has changed. Ensure that we update the DesiredSize that the content
        // reports, because the desired size may depend on the size and/or shape of the viewport.

        UwpLayoutBridge::get().invalidateMeasure( _pContentWrapper );
	}

    virtual void _uwpViewChanged()
	{
        // OutputDebugString( L"ScrollViewCore._uwpViewChanged()\n" );

        updateVisibleRect();
	}

    virtual void updateVisibleRect()
    {
        // a scroll operation has changed the visible rect.
        P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );
        if(pOuter!=nullptr)
        {
            Rect visibleRect;
            visibleRect.x = _pScrollViewer->HorizontalOffset;
            visibleRect.y = _pScrollViewer->VerticalOffset;
            visibleRect.width = _pScrollViewer->ViewportWidth;
            visibleRect.height = _pScrollViewer->ViewportHeight;

            pOuter->_setVisibleClientRect(visibleRect);
        }        
    }

private:
    
    


    ref class ScrollViewCoreEventForwarder : public ViewCoreEventForwarder
	{
	internal:
		ScrollViewCoreEventForwarder(ChildViewCore* pParent)
            : ViewCoreEventForwarder(pParent)
		{
		}
        
	public:
		void viewChanged( Platform::Object^ pSender,  ::Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ pArgs)
		{
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			P<ScrollViewCore> pViewCore = cast<ScrollViewCore>( getViewCoreIfAlive() );
			if(pViewCore!=nullptr)
				pViewCore->_uwpViewChanged();

            BDN_WINUWP_TO_PLATFORMEXC_END
		}
	};
    
    class ContentWrapperLayoutDelegate : public Base, BDN_IMPLEMENTS IUwpLayoutDelegate
    {
    public:
        ContentWrapperLayoutDelegate( ScrollView* pView)
            : _viewWeak(pView)
        {
        }

        Size uwpMeasureOverride(const Size& availableSpace ) override
        {
            // OutputDebugString( String( "ContentWrapperLayoutDelegate.measureOverride("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+"\n" ).asWidePtr() );

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

            // OutputDebugString( String( "/ContentWrapperLayoutDelegate.measureOverride\n" ).asWidePtr() );
            
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
            // OutputDebugString( String( "ContentWrapperLayoutDelegate.arrangeOverride("+std::to_string(finalSize.width)+", "+std::to_string(finalSize.height)+"\n" ).asWidePtr() );

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

                            // OutputDebugString( String( "ScrollView content view "+String(pContentElement->GetType()->ToString()->Data())+" arrange: "+std::to_string(contentBounds.x)+", "+std::to_string(contentBounds.y)+", "+std::to_string(contentBounds.width)+", "+std::to_string(contentBounds.height)+"\n" ).asWidePtr() );
                            
					        ::Windows::Foundation::Rect winContentBounds = rectToUwpRect( contentBounds );
                            				
					        pContentElement->Arrange(winContentBounds);

                            // XXX
                            double width = pContentElement->ActualWidth;
                            double height = pContentElement->ActualHeight;

                            // OutputDebugString( String( "Resulting size: "+std::to_string(width)+" x "+std::to_string(height)+"\n" ).asWidePtr() );
				        }
                    }
                }
                
                P<ScrollViewCore> pCore = tryCast<ScrollViewCore>( pView->getViewCore() );
				if(pCore!=nullptr)
                {
					pCore->_pScrollViewer->InvalidateScrollInfo();
                    pCore->updateVisibleRect();
                }
            }     

            // OutputDebugString( String( "/ContentWrapperLayoutDelegate.arrangeOverride()\n" ).asWidePtr() );

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
