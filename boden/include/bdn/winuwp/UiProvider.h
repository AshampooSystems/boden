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
    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;


	double getUiScaleFactor() const
	{
		return DisplayInformation::GetForCurrentView().RawPixelsPerViewPixel;
	}


	Rect getScreenWorkArea() const
	{
		Windows::Foundation::Rect bounds = ApplicationView.GetForCurrentView().VisibleBounds;

		double scaleFactor = _pUiProvider->getUiScaleFactor();

		return Rect( bounds.x*scaleFactor, bounds.y*scaleFactor, bounds.width*scaleFactor, bounds.height*scaleFactor );		
	}
	

	int uiLengthToPixels(const UiLength& uiLength) const override
	{
		if(uiLength.unit==UiLength::sem)
			return std::lround( uiLength.value * getSemSizeForUiScaleFactor(uiScaleFactor) );

		else if(uiLength.unit==UiLength::pixel96)
		{
			// See UiLength documentation for more information about the pixel96 unit
			// and why this is correct.
			return std::lround( uiLength.value * uiScaleFactor );
		}

		else if(uiLength.unit==UiLength::realPixel)
			return std::lround( uiLength.value );

		else
			throw InvalidArgumentError("Invalid UiLength unit passed to Win32UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
	}

		if(uiLength.unit==UiLength::Unit::sem)
		{
			
		}
		else if(uiLength.unit==UiLength::Unit::pixel96)
		{
			return uiLength.value * getUiScaleFactor();
		}
		else if(uiLength.unit==UiLength::Unit::realPixels)
		{
			// nothing to convert
			return uiLength.value;			
		}
		else

	}
	

	Margin uiMarginToPixelMargin(const UiMargin& margin) const
	{
		return Margin(
			uiLengthToPixels(margin.top),
			uiLengthToPixels(margin.right),
			uiLengthToPixels(margin.bottom),
			uiLengthToPixels(margin.left) );
	}

    static P<UiProvider> get()
    {
        static SafeInit<UiProvider> init;
        
        return init.get();
    }

};

}
}


#endif
