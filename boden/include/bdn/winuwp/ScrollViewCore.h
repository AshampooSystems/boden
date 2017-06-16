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

		doSizingInfoUpdateOnNextLayout();

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
        _pScrollViewer->Content = pUiElement;

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
    }


    XXX - add content wrap view / possibly generic wrap view that holds a boden hierarchy
        - implement MeasureOverride for that and ensure that wrap view reports the correct sizes
          to the surrounding uwp environment
        - add the wrap view to the scroll viewer => automatic layout
        - add the actual content view to the wrap view

    
protected:    
	::Windows::UI::Xaml::Controls::ScrollViewer^ _pScrollViewer;	
};

}
}

#endif
