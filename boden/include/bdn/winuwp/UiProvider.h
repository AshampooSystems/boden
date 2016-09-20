#ifndef BDN_WINUWP_UiProvider_H_
#define BDN_WINUWP_UiProvider_H_

#include <bdn/IUiProvider.h>

#include <bdn/winuwp/platformError.h>
#include <bdn/winuwp/util.h>

namespace bdn
{
namespace winuwp
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
	UiProvider()
	{		
        BDN_WINUWP_TO_STDEXC_BEGIN;

        // XXX Todo: need to properly determine base font size.
		_semDips = 15;

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

        BDN_WINUWP_TO_STDEXC_END;
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
        BDN_WINUWP_TO_STDEXC_BEGIN;

		Windows::Foundation::Rect bounds =  Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->VisibleBounds;
		
		return uwpRectToRect(bounds);

        BDN_WINUWP_TO_STDEXC_END;
	}
	
	double	uiLengthToDips(const UiLength& uiLength) const
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		if(uiLength.unit==UiLength::sem)
			return uiLength.value * _semDips;

		else if(uiLength.unit==UiLength::dip)
            return uiLength.value;
		
        else if(uiLength.unit==UiLength::realPixel)
        {
			// See UiLength documentation for more information about the dip unit
			// and why this is correct.
			return uiLength.value / _uiScaleFactor;
		}

		else
			throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );

        BDN_WINUWP_TO_STDEXC_END;
	}
	

	Margin			uiMarginToDipMargin(const UiMargin& margin) const
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		return Margin(
			uiLengthToDips(margin.top),
			uiLengthToDips(margin.right),
			uiLengthToDips(margin.bottom),
			uiLengthToDips(margin.left) );

        BDN_WINUWP_TO_STDEXC_END;
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
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			MutexLock lock(_parentMutex);
			
			if(_pParentWeak!=nullptr)
				_pParentWeak->updateUiScaleFactor(pDisplayInfo);

            BDN_WINUWP_TO_PLATFORMEXC_END
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
        BDN_WINUWP_TO_STDEXC_BEGIN;

		_uiScaleFactor = pDisplayInfo->RawPixelsPerViewPixel;		
		
		BDN_WINUWP_TO_STDEXC_END;
	}

	
	double _uiScaleFactor = 0;
	double _semDips = 0;

	EventForwarder^ _pEventForwarder;
};

}
}


#endif
