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

        // our layout is completely controlled by Windows. So we do not want to cause
        // a boden layout after each UWP layout
        setUwpLayoutTriggersOurLayout(false);

		_pScrollViewer = dynamic_cast< ::Windows::UI::Xaml::Controls::ScrollViewer^ >( getFrameworkElement() );

        // we want to let the scroll viewer handle the scroll bars and layouting automatically.
        // It is compatible with our own layout system.
        // However, we still want to handle the layout ourselves within the content.
        // To achieve that we have to give the scroll viewer has an UWP content panel
        // that wraps the actual content view and forwards layout and measure calls accordingly.
        _pContentWrapper = ref new ScrollViewContentViewWrapper(this);

        _pScrollViewer->Content = _pContentWrapper;

        setHorizontalScrollingEnabled( pOuter->horizontalScrollingEnabled() );
        setVerticalScrollingEnabled( pOuter->verticalScrollingEnabled() );
        
        BDN_WINUWP_TO_STDEXC_END;
	}

    ~ScrollViewCore()
    {
        if(_pScrollViewer!=nullptr)
        {
            ScrollViewContentViewWrapper^ pContentWrapper = dynamic_cast<ScrollViewContentViewWrapper^>(_pScrollViewer->Content);

            if(pContentWrapper!=nullptr)
                pContentWrapper->detachFromScrollViewCore();
        }
    }

    

	void setPadding(const Nullable<UiMargin>& pad) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// Apply the padding to the control, so that the content is positioned accordingly.
        UiMargin uiPadding;
        if(!pad.isNull())
            uiPadding = pad;

		Margin padding = uiMarginToDipMargin(uiPadding);

		scheduleSizingInfoUpdateAfterNextUwpLayout();

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

        _pContentWrapper->Content = pUiElement;

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

        // note that a UWP layout operation triggers a delayed layoutUpdated event for the view.
        // That would normally cause a Boden layout to follow. However, we have used
        // setUwpLayoutTriggersOurLayout in our constructor to disable that completely, since
        // we need the layout operations to happen during the normal Windows layout cycle
        // (so that scroll bars etc are adapted accordingly).

        _pScrollViewer->UpdateLayout();
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

private:    

    ref class ScrollViewContentViewWrapper sealed : public ::Windows::UI::Xaml::Controls::ContentControl
    {
    private:
        ScrollViewContentViewWrapper(ScrollViewCore* pScrollViewCore)
        {
            _pScrollViewCore = pScrollViewCore;
        }

    public:

        void detachFromScrollViewCore()
        {
            _pScrollViewCore = nullptr;
        }
       
    protected:

        virtual ::Windows::Foundation::Size MeasureOverride(::Windows::Foundation::Size winAvailableSize) override
        {
            P<View> pContentView = getWrappedContentView();
            if(pContentView!=nullptr)
            {
                Size availableSpace = Size::none();

                if(std::isfinite(winAvailableSize.Width))
                    availableSpace.width = winAvailableSize.Width;
                
                if(std::isfinite(winAvailableSize.Height))
                    availableSpace.height = winAvailableSize.Height;

                Size size = pContentView->calcPreferredSize(availableSpace);

                return sizeToUwpSize( size );
            }
            else
                return ::Windows::Foundation::Size(0,0);
        }

        virtual ::Windows::Foundation::Size ArrangeOverride(::Windows::Foundation::Size winFinalSize) override
        {
            // make the content view the same size as us
            Rect contentRect( Point(0,0), uwpSizeToSize(winFinalSize) );
            
            P<View> pContentView = getWrappedContentView();
            if(pContentView!=nullptr)
            {
                // It is important to let the child view know that we are currently in a UWP layout operation.
                // That means that it can use Arrange directly.
                ChildViewCore::InUwpLayoutOperation_ inOp(pContentView);

                // now assign the new content rect to the view
                Rect adjustedContentRect = pContentView->adjustAndSetBounds(contentRect);
            
                return sizeToUwpSize( adjustedContentRect.getSize() );
            }
            else
                return winFinalSize;
        }

    private:
        P<View> getWrappedContentView()
        {
            if(_pScrollViewCore!=nullptr)
            {
                P<ScrollView> pScrollView = cast<ScrollView>( _pScrollViewCore->getOuterViewIfStillAttached() );
                if(pScrollView!=nullptr)
                    return pScrollView->getContentView();
            }

            return nullptr;
        }


        // weak by design
        ScrollViewCore* _pScrollViewCore;

        friend class ScrollViewCore;
    };

    ::Windows::UI::Xaml::Controls::ScrollViewer^ _pScrollViewer;
    ScrollViewContentViewWrapper^                _pContentWrapper;
    bool                                         _ignoreUwpLayoutUpdated = false;
};

}
}

#endif
