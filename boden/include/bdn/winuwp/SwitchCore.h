#ifndef BDN_WINUWP_SwitchCore_H_
#define BDN_WINUWP_SwitchCore_H_

#include <bdn/Switch.h>
#include <bdn/winuwp/ChildViewCore.h>

namespace bdn
{
namespace winuwp
{

template <class T>
class SwitchCore : public ChildViewCore, BDN_IMPLEMENTS ISwitchCore
{
private:
	static ::Windows::UI::Xaml::Controls::StackPanel^ _createSwitch(T* pOuter)
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        // We have to use this wonderful StackPanel container to implement correct padding on the checkbox.
        // If padding is applied to the CheckBox control directly, the checkbox rect is not padded correctly
        // yielding weird layouts with displaced text labels.
        ::Windows::UI::Xaml::Controls::StackPanel^ container = ref new ::Windows::UI::Xaml::Controls::StackPanel();
		::Windows::UI::Xaml::Controls::ToggleSwitch^ toggle = ref new ::Windows::UI::Xaml::Controls::ToggleSwitch();
		container->Children->Append(toggle);

		return container;

        BDN_WINUWP_TO_STDEXC_END;
	}

public:

	/** Used internally.*/
	ref class SwitchCoreEventForwarder : public ViewCoreEventForwarder
	{
	internal:
		SwitchCoreEventForwarder(SwitchCore* pParent)
			: ViewCoreEventForwarder(pParent)
		{
		}

		SwitchCore* getSwitchCoreIfAlive()
		{
			return dynamic_cast<SwitchCore*>( getViewCoreIfAlive() );
		}

	public:
		void toggled(Object ^sender, ::Windows::UI::Xaml::RoutedEventArgs^ pArgs)
		{
			BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			SwitchCore* pSwitchCore = getSwitchCoreIfAlive();
			if (pSwitchCore != nullptr)
				pSwitchCore->_toggled();

			BDN_WINUWP_TO_PLATFORMEXC_END
		}

	private:
		ChildViewCore* _pParentWeak;
	};


	SwitchCore(	T* pOuter)
		: ChildViewCore(pOuter, _createSwitch(pOuter), ref new SwitchCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pContainer = dynamic_cast< ::Windows::UI::Xaml::Controls::StackPanel^ >( getFrameworkElement() );
		_pToggle = dynamic_cast< ::Windows::UI::Xaml::Controls::ToggleSwitch^ >( _pContainer->Children->GetAt(0) );
		_pToggle->MinWidth = 0;
		_pToggle->IsOn = (bool)pOuter->on();

		SwitchCoreEventForwarder^ pEventForwarder = dynamic_cast<SwitchCoreEventForwarder^>( getViewCoreEventForwarder() );
		_pToggle->Toggled += ref new ::Windows::UI::Xaml::RoutedEventHandler( pEventForwarder, &SwitchCoreEventForwarder::toggled );
		
		setPadding( pOuter->padding() );
		setLabel( pOuter->label() );
		setOn( pOuter->on() );

        BDN_WINUWP_TO_STDEXC_END;
	}


    void setPadding(const Nullable<UiMargin>& pad) override
	{
		BDN_WINUWP_TO_STDEXC_BEGIN;
        
		// Apply the padding to the control, so that the content is positioned accordingly.
        UiMargin uiPadding;
        if(pad.isNull())
        {
            // we should use a default padding that looks good.
            // Xaml uses zero padding as the default, so we cannot use their
            // default value. So we choose our own default that matches the
            // normal aesthetic of Windows apps.
            uiPadding = UiMargin(UiLength::sem(0.4), UiLength::sem(1) );
        }
        else
            uiPadding = pad;

		Margin padding = uiMarginToDipMargin(uiPadding);
        
        // UWP also uses DIPs => no conversion necessary

        // Note that the padding must be set on the button Content to have an effect. The button
        // class itself seems to completely ignore it.
        _containerPadding = ::Windows::UI::Xaml::Thickness(
			padding.left,
			padding.top,
			padding.right,
			padding.bottom );

        _pContainer->Padding = _containerPadding;

        BDN_WINUWP_TO_STDEXC_END;
	}

	void setLabel(const String& label)
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		::Windows::UI::Xaml::Controls::TextBlock^ pLabel = ref new ::Windows::UI::Xaml::Controls::TextBlock();

		pLabel->Text = ref new ::Platform::String( label.asWidePtr() );
		// we cannot simply schedule a sizing info update here. The desired size of the control will still
		// be outdated when the sizing happens.
		// Instead we wait for the "layout updated" event that will happen soon after we set the
		// content. That is when we update our sizing info.
        
        _pToggle->Header = pLabel;

        BDN_WINUWP_TO_STDEXC_END;
	}

	void setOn(const bool& on)
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pToggle->IsOn = on;

        BDN_WINUWP_TO_STDEXC_END;
	}

	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		int brk=1;
		return ChildViewCore::calcPreferredSize(availableSpace);
	}


protected:

	void _setToggledInternal(const bool on)
	{
		P<View> pOuterView = getOuterViewIfStillAttached();
		if (pOuterView != nullptr) {
			cast<T>(pOuterView)->setOn( on );
		}		
	}

	void _toggled()
	{
		BDN_WINUWP_TO_STDEXC_BEGIN;

		_setToggledInternal(_pToggle->IsOn);

		BDN_WINUWP_TO_STDEXC_END;
	}
   
    ::Windows::UI::Xaml::Controls::StackPanel^   _pContainer;
    ::Windows::UI::Xaml::Controls::ToggleSwitch^ _pToggle;

    ::Windows::UI::Xaml::Thickness               _containerPadding;
};

}
}

#endif
