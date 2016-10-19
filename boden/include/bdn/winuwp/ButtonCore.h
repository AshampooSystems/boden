#ifndef BDN_WINUWP_ButtonCore_H_
#define BDN_WINUWP_ButtonCore_H_

#include <bdn/Button.h>
#include <bdn/winuwp/ChildViewCore.h>

namespace bdn
{
namespace winuwp
{

class ButtonCore : public ChildViewCore, BDN_IMPLEMENTS IButtonCore
{
private:
	static ::Windows::UI::Xaml::Controls::Button^ _createButton(Button* pOuter)
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		return ref new ::Windows::UI::Xaml::Controls::Button();		

        BDN_WINUWP_TO_STDEXC_END;
	}

public:
	
	/** Used internally.*/
	ref class ButtonCoreEventForwarder : public ViewCoreEventForwarder
	{
	internal:
		ButtonCoreEventForwarder(ButtonCore* pParent)
			: ViewCoreEventForwarder(pParent)
		{
		}

		ButtonCore* getButtonCoreIfAlive()
		{
			return dynamic_cast<ButtonCore*>( getViewCoreIfAlive() );
		}

	public:
		void clicked(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ pArgs)
		{
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			ButtonCore* pButtonCore = getButtonCoreIfAlive();
			if(pButtonCore!=nullptr)
				pButtonCore->_clicked();			

            BDN_WINUWP_TO_PLATFORMEXC_END
		}		

	private:
		ChildViewCore* _pParentWeak;
	};


	ButtonCore(	Button* pOuter)
		: ChildViewCore(pOuter, _createButton(pOuter), ref new ButtonCoreEventForwarder(this) )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		_pButton = dynamic_cast< ::Windows::UI::Xaml::Controls::Button^ >( getFrameworkElement() );

		ButtonCoreEventForwarder^ pEventForwarder = dynamic_cast<ButtonCoreEventForwarder^>( getViewCoreEventForwarder() );

		_pButton->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler( pEventForwarder, &ButtonCoreEventForwarder::clicked );
		
		setPadding( pOuter->padding() );
		setLabel( pOuter->label() );

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

		_doSizingInfoUpdateOnNextLayout = true;		

        // UWP also uses DIPs => no conversion necessary

        // Note that the padding must be set on the button Content to have an effect. The button
        // class itself seems to completely ignore it.
        _contentPadding = ::Windows::UI::Xaml::Thickness(
			padding.left,
			padding.top,
			padding.right,
			padding.bottom );
        _applyContentPadding = true;

        if(_pButton->Content!=nullptr)
            ((::Windows::UI::Xaml::Controls::TextBlock^)_pButton->Content)->Padding = _contentPadding;

        
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

		_doSizingInfoUpdateOnNextLayout = true;		

        _pButton->Content = pLabel;		

        if(_applyContentPadding)
            pLabel->Padding = _contentPadding;


        BDN_WINUWP_TO_STDEXC_END;
	}


protected:

	void _clicked()
	{
        P<View> pOuterView = getOuterViewIfStillAttached();
        if(pOuterView!=nullptr)
        {
		    ClickEvent evt(pOuterView);
		    cast<Button>(pOuterView)->onClick().notify(evt);
        }
	}

	void _layoutUpdated() override
	{
		if(_doSizingInfoUpdateOnNextLayout)
		{
			_doSizingInfoUpdateOnNextLayout = false;

            P<View> pOuterView = getOuterViewIfStillAttached();
            if(pOuterView!=nullptr)
			    pOuterView->needSizingInfoUpdate();
		}
	}

   
	::Windows::UI::Xaml::Controls::Button^ _pButton;

    ::Windows::UI::Xaml::Thickness _contentPadding;
    bool                           _applyContentPadding = false;

	double      _doSizingInfoUpdateOnNextLayout = true;
	
};

}
}

#endif
