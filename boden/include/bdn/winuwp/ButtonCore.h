#ifndef BDN_WINUWP_ButtonCore_H_
#define BDN_WINUWP_ButtonCore_H_

#include <bdn/Button.h>
#include <bdn/winuwp/ViewCore.h>

namespace bdn
{
namespace winuwp
{

class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
{
private:
	static ::Windows::UI::Xaml::Controls::Button^ _createButton(Button* pOuter)
	{
		return ref new ::Windows::UI::Xaml::Controls::Button();		
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
			ButtonCore* pButtonCore = getButtonCoreIfAlive();
			if(pButtonCore!=nullptr)
				pButtonCore->_clicked();			
		}		

	private:
		ViewCore* _pParentWeak;
	};


	ButtonCore(	Button* pOuter)
		: ViewCore(pOuter, _createButton(pOuter), ref new ButtonCoreEventForwarder(this) )
	{
		_pButton = dynamic_cast< ::Windows::UI::Xaml::Controls::Button^ >( getFrameworkElement() );

		ButtonCoreEventForwarder^ pEventForwarder = dynamic_cast<ButtonCoreEventForwarder^>( getViewCoreEventForwarder() );

		_pButton->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler( pEventForwarder, &ButtonCoreEventForwarder::clicked );
		
		setPadding( pOuter->padding() );
		setLabel( pOuter->label() );
	}

	void setPadding(const UiMargin& uiPadding) override
	{
		// Apply the padding to the control, so that the content is positioned accordingly.
		Margin padding = UiProvider::get().uiMarginToPixelMargin(uiPadding);

		_doSizingInfoUpdateOnNextLayout = true;		

		_pButton->Padding = ::Windows::UI::Xaml::Thickness( padding.left, padding.top, padding.right, padding.bottom );
	}

	void setLabel(const String& label)
	{
		::Windows::UI::Xaml::Controls::TextBlock^ pLabel = ref new ::Windows::UI::Xaml::Controls::TextBlock();

		pLabel->Text = ref new ::Platform::String( label.asWidePtr() );

		// we cannot simply schedule a sizing info update here. The desired size of the control will still
		// be outdated when the sizing happens.
		// Instead we wait for the "layout updated" event that will happen soon after we set the
		// content. That is when we update our sizing info.

		_doSizingInfoUpdateOnNextLayout = true;		

		_pButton->Content = pLabel;		
	}


protected:

	void _clicked()
	{
		ClickEvent evt(getOuterView());

		cast<Button>(getOuterView())->onClick().notify(evt);
	}

	void _layoutUpdated() override
	{
		if(_doSizingInfoUpdateOnNextLayout)
		{
			_doSizingInfoUpdateOnNextLayout = false;

			getOuterView()->needSizingInfoUpdate();
		}
	}

	::Windows::UI::Xaml::Controls::Button^ _pButton;

	double _doSizingInfoUpdateOnNextLayout = true;
	
};

}
}

#endif
