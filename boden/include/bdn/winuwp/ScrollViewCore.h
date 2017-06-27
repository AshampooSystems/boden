#ifndef BDN_WINUWP_ScrollViewCore_H_
#define BDN_WINUWP_ScrollViewCore_H_

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>
#include <bdn/winuwp/ChildViewCore.h>

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
        _pContentWrapper = ref new UwpPanelWithCustomLayout( newObj<ContentWrapperLayoutDelegate>(pOuter) );

        _pScrollViewer->Content = _pContentWrapper;

        setHorizontalScrollingEnabled( pOuter->horizontalScrollingEnabled() );
        setVerticalScrollingEnabled( pOuter->verticalScrollingEnabled() );
        
        BDN_WINUWP_TO_STDEXC_END;
	}


    

	void setPadding(const Nullable<UiMargin>& pad) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// Apply the padding to the control, so that the content is positioned accordingly.
        UiMargin uiPadding;
        if(!pad.isNull())
            uiPadding = pad;

		Margin padding = uiMarginToDipMargin(uiPadding);
        
		// UWP also uses DIPs => no conversion necessary

		_pScrollViewer->Padding = ::Windows::UI::Xaml::Thickness(
			padding.left,
			padding.top,
			padding.right,
			padding.bottom );
        
        BDN_WINUWP_TO_STDEXC_END;
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
        // cause a UWP layout. That will cause our ArrangeOverride method to be called,
        // which in turn will arrange the content view.
        
        _pScrollViewer->UpdateLayout();
    }


    

    void needSizingInfoUpdate() override
    {
        ChildViewCore::needSizingInfoUpdate();


        // also invalidate the measurements of the content wrapper
        BDN_WINUWP_TO_STDEXC_BEGIN;

        try
        {
		    _pContentWrapper->InvalidateMeasure();
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this.
        }

        BDN_WINUWP_TO_STDEXC_END;
    }


    
    void needLayout() override
    {
        ChildViewCore::needSizingInfoUpdate();


        // also invalidate the layout of the content wrapper
        BDN_WINUWP_TO_STDEXC_BEGIN;

        try
        {
		    _pContentWrapper->InvalidateArrange();
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this.
        }

        BDN_WINUWP_TO_STDEXC_END;
    }

    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        return ChildViewCore::calcPreferredSize(availableSpace);
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

        // the scroll viewer's size has changed. Ensurethat we update the DesiredSize that the content
        // reports, because the desired size may depend on the size and/or shape of the viewport.
        
        _pContentWrapper->InvalidateMeasure();
	}

private:
    
    class ContentWrapperLayoutDelegate : public Base, BDN_IMPLEMENTS IUwpLayoutDelegate
    {
    public:
        ContentWrapperLayoutDelegate( ScrollView* pView)
            : _viewWeak(pView)
        {
        }

        ::Windows::Foundation::Size measureOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, ::Windows::Foundation::Size winAvailableSize ) override
        {
            // XXX
            OutputDebugString( String( "ContentWrapperLayoutDelegate.measureOverride("+std::to_string(winAvailableSize.Width)+", "+std::to_string(winAvailableSize.Height)+"\n" ).asWidePtr() );

            P<ScrollView> pView = _viewWeak.toStrong();

            if(pView!=nullptr)
            {
                P<View> pContentView = pView->getContentView();
                if(pContentView!=nullptr)
                {
                    pContentView->_doUpdateSizingInfo();
                                        
                    Margin padding;                    
                    Nullable<UiMargin> pad = pView->padding();
                    if(!pad.isNull())
                        padding = pView->uiMarginToDipMargin(pad);            

                    Margin contentMargin = pContentView->uiMarginToDipMargin( pContentView->margin() );

                    Size nonContentSize = Size(0,0) + contentMargin + padding;

                    // forward this to the outer view.
                    Size availableSpace = Size::none();

                    if( std::isfinite(winAvailableSize.Width) )
                    {
                        availableSpace.width = winAvailableSize.Width - nonContentSize.width;
                        if(availableSpace.width<0)
                            availableSpace.width = 0;
                    }

                    if( std::isfinite(winAvailableSize.Height) )
                    {
                        availableSpace.height = winAvailableSize.Height - nonContentSize.height;
                        if(availableSpace.height<0)
                            availableSpace.height = 0;
                    }

                    Size resultSize = pContentView->calcPreferredSize( availableSpace );

                    resultSize += nonContentSize;

                    OutputDebugString( String( "/ContentWrapperLayoutDelegate.measureOverride\n" ).asWidePtr() );

                    return sizeToUwpSize(resultSize);
                }
            }

            OutputDebugString( String( "/ContentWrapperLayoutDelegate.measureOverride\n" ).asWidePtr() );
            
            return ::Windows::Foundation::Size(0, 0);
        }
        
        ::Windows::Foundation::Size arrangeOverride(::Windows::UI::Xaml::Controls::Panel^ pPanel, ::Windows::Foundation::Size winFinalSize ) override
        {
            // XXX
            OutputDebugString( String( "ContentWrapperLayoutDelegate.arrangeOverride("+std::to_string(winFinalSize.Width)+", "+std::to_string(winFinalSize.Height)+"\n" ).asWidePtr() );

            P<ScrollView> pView = _viewWeak.toStrong();

            if(pView!=nullptr)
            {
                P<View> pContentView = pView->getContentView();
                if(pContentView!=nullptr)
                {
                    // make the content view the same size as us
                    Rect contentRect( Point(0,0), uwpSizeToSize(winFinalSize) );

                    Margin padding;                    
                    Nullable<UiMargin> pad = pView->padding();
                    if(!pad.isNull())
                        padding = pView->uiMarginToDipMargin(pad);            

                    Margin contentMargin = pContentView->uiMarginToDipMargin( pContentView->margin() );

                    contentRect -= padding;
                    contentRect -= contentMargin;
                    
                    pContentView->adjustAndSetBounds(contentRect);
                }
            }            

            return winFinalSize;
        }

    protected:
        WeakP<ScrollView> _viewWeak;
    };
    friend class ContentWrapperLayoutDelegate;


    ::Windows::UI::Xaml::Controls::ScrollViewer^ _pScrollViewer;
    UwpPanelWithCustomLayout^                    _pContentWrapper;
    bool                                         _ignoreUwpLayoutUpdated = false;
};

}
}

#endif
