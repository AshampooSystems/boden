#ifndef BDN_WINUWP_UiProvider_H_
#define BDN_WINUWP_UiProvider_H_

#include <bdn/IUiProvider.h>
#include <bdn/ViewTextUi.h>

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
        // The default font size for controls is documented as 11 DIPs. So we use that in the meantime.
		_semDips = 11;

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


    double getSemSizeDips() const
    {
        return _semDips;
    }



	Rect			getScreenWorkArea() const
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		Windows::Foundation::Rect bounds =  Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->VisibleBounds;
		
		return uwpRectToRect(bounds);

        BDN_WINUWP_TO_STDEXC_END;
	}

    
    P<ITextUi> getTextUi() override
    {
        {
            MutexLock lock( _textUiInitMutex );
            if(_pTextUi==nullptr)
                _pTextUi = newObj< ViewTextUi >();
        }

        return _pTextUi;
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

    Mutex           _textUiInitMutex;
    P<ViewTextUi>   _pTextUi;

	EventForwarder^ _pEventForwarder;
};

}
}


#endif
