#ifndef BDN_WINUWP_UiProvider_H_
#define BDN_WINUWP_UiProvider_H_

#include <bdn/IUiProvider.h>

namespace bdn
{
namespace winuwp
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
	UiProvider()
	{
		
		Windows::UI::ViewManagement::ApplicationView^ pAppView = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();


		Windows::UI::Xaml::Window^ pWindow = Windows::UI::Xaml::Window::Current;


		Windows::Graphics::Display::DisplayInformation^ pDisplayInfo = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

		_pEventForwarder = ref new EventForwarder(this);

		pDisplayInfo->DpiChanged +=
			ref new Windows::Foundation::TypedEventHandler
				<Windows::Graphics::Display::DisplayInformation^,
				 Platform::Object^>
				(_pEventForwarder, &EventForwarder::dpiChanged);

		updateUiScaleFactor( pDisplayInfo );
	}

	~UiProvider()
	{
		_pEventForwarder->dispose();		
	}
    
    String			getName() const override;
    
    P<IViewCore>	createViewCore(const String& coreTypeName, View* pView) override;


	double			getUiScaleFactor() const
	{
		return _uiScaleFactor;
	}


	Rect			getScreenWorkArea() const
	{
		Windows::Foundation::Rect bounds =  Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->VisibleBounds;
		
		return Rect(
			std::lround( bounds.X * _uiScaleFactor ),
			std::lround( bounds.Y * _uiScaleFactor ),
			std::lround( bounds.Width * _uiScaleFactor ),
			std::lround( bounds.Height * _uiScaleFactor ) );		
	}
	

	int				uiLengthToPixels(const UiLength& uiLength) const
	{
		if(uiLength.unit==UiLength::sem)
			return std::lround( uiLength.value * _semPixels );

		else if(uiLength.unit==UiLength::pixel96)
		{
			// See UiLength documentation for more information about the pixel96 unit
			// and why this is correct.
			return std::lround( uiLength.value * _uiScaleFactor );
		}

		else if(uiLength.unit==UiLength::realPixel)
			return std::lround( uiLength.value );

		else
			throw InvalidArgumentError("Invalid UiLength unit passed to Win32UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
	}
	

	Margin			uiMarginToPixelMargin(const UiMargin& margin) const
	{
		return Margin(
			uiLengthToPixels(margin.top),
			uiLengthToPixels(margin.right),
			uiLengthToPixels(margin.bottom),
			uiLengthToPixels(margin.left) );
	}


    static UiProvider& get();


protected:
	

	ref class EventForwarder sealed
	{
	internal:
		EventForwarder(UiProvider* pParent)
		{
			_pParentWeak = pParent;
		}

	public:

		void dpiChanged(	Windows::Graphics::Display::DisplayInformation^ pDisplayInfo,
						Platform::Object^ args)
		{
			MutexLock lock(_parentMutex);
			
			if(_pParentWeak!=nullptr)
				_pParentWeak->updateUiScaleFactor(pDisplayInfo);
		}

		void dispose()
		{
			MutexLock lock(_parentMutex);
			_pParentWeak = nullptr;
		}

	internal:
		Mutex		_parentMutex;
		UiProvider* _pParentWeak;
	};


	void updateUiScaleFactor( Windows::Graphics::Display::DisplayInformation^ pDisplayInfo )
	{
		_uiScaleFactor = pDisplayInfo->RawPixelsPerViewPixel;		
		
		// Todo: need to properly determine base font size.
		_semPixels = 15 * _uiScaleFactor;
	}

	
	double _uiScaleFactor = 0;
	double _semPixels = 0;

	EventForwarder^ _pEventForwarder;
};

}
}


#endif
