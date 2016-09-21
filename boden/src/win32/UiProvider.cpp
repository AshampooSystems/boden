#include <bdn/init.h>
#include <bdn/win32/UiProvider.h>

#include <bdn/win32/ButtonCore.h>
#include <bdn/win32/WindowCore.h>
#include <bdn/win32/ContainerViewCore.h>
#include <bdn/win32/TextViewCore.h>
#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/win32/win32Error.h>

#include <ShellScalingAPI.h>

namespace bdn
{

P<IUiProvider> getPlatformUiProvider()
{
	return &bdn::win32::UiProvider::get();
}

}

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );

UiProvider::UiProvider()
{		
	// the default UI font on Windows changed multiple times over the years.
	// There are several deprecated legacy ways to get it (the font name "MS Shell Dlg",
	// or GetStockObject(DEFAULT_GUI_FONT) or the DS_SHELLFONT attribute.
	// But all of these return old oudated fonts on modern Windows versions.
	// This thread contains a good overview over the history:
	// http://stackoverflow.com/questions/6057239/which-font-is-the-default-for-mfc-dialog-controls
	
	// The "current" recommended way is to do this:

	NONCLIENTMETRICS metrics = {sizeof(NONCLIENTMETRICS), 0};
		
	if( ! ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, FALSE) )
	{
		BDN_WIN32_throwLastError( ErrorFields().add("func", "SystemParametersInfo(SPI_GETNONCLIENTMETRICS)")
											.add("action", "UiProvider constructor") );
	}

	_defaultUiFontInfo = metrics.lfMessageFont;

	// we want nice quality
	_defaultUiFontInfo.lfQuality = CLEARTYPE_QUALITY;

	// the default UI font will have a size that is already scaled according to the UI scalefactor
	// of the default monitor.
	POINT ptZero = { 0, 0 };
	double primaryUiScaleFactor = getUiScaleFactorForMonitor( MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY) );

	// store the UI font size at scale factor 1 as a double, so that we do not get rounding errors
	// when we scale back up.
	_defaultUiFontSizeAtScaleFactor1 = _defaultUiFontInfo.lfHeight / primaryUiScaleFactor;

	_defaultUiFontInfo.lfHeight = std::lround(_defaultUiFontSizeAtScaleFactor1);

    // create the font for scale factor 1
	_pDefaultUiFont = newObj<Font>(_defaultUiFontInfo);

    double fontSize = _pDefaultUiFont->getSizePixels();

    // scale factor 1 = DIP	
	_semSizeDips = fontSize;	

	// add the font to our map
	FontSpec spec;
	_fontMap[spec] = _pDefaultUiFont;
}


P<Font> UiProvider::getFontForView(View* pView, double uiScaleFactor)
{
	FontSpec spec;
	spec.uiScaleFactor = uiScaleFactor;

	P<Font> pFont;

	auto it = _fontMap.find( spec );
	if(it==_fontMap.end())
	{
		// font is not yet in the map
		pFont = createFont(spec);		

		_fontMap[spec] = pFont;
	}
	else
		pFont = it->second;

	return pFont;
}

P<Font> UiProvider::createFont(const FontSpec& spec)
{
	LOGFONT info = _defaultUiFontInfo;

	info.lfHeight = std::lround( _defaultUiFontSizeAtScaleFactor1*spec.uiScaleFactor );

	return newObj<Font>(info);
}


P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
	if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
		return newObj<ContainerViewCore>( cast<ContainerView>(pView) );

	else if(coreTypeName == Button::getButtonCoreTypeName() )
		return newObj<ButtonCore>( cast<Button>(pView) );

	else if(coreTypeName == Window::getWindowCoreTypeName() )
		return newObj<WindowCore>( cast<Window>(pView) );

    else if(coreTypeName == TextView::getTextViewCoreTypeName() )
		return newObj<TextViewCore>( cast<TextView>(pView) );

	else	
		throw ViewCoreTypeNotSupportedError(coreTypeName);
}




double UiProvider::getUiScaleFactorForTopLevelWindow(HWND window)
{	
	HMONITOR	monitorHandle = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

	return getUiScaleFactorForMonitor(monitorHandle);
}

double UiProvider::getUiScaleFactorForMonitor(HMONITOR monitorHandle)
{
	UINT dpiX = 0;
	UINT dpiY = 0;
	::GetDpiForMonitor(monitorHandle, MDT_EFFECTIVE_DPI, &dpiX ,&dpiY);

	// note that the DPI we get here is not a real physical DPI. Rather it is a more complicated
	// value that takes into account the device type and even user settings.
	// It is basically the UI scale factor that Windows wants us to use, multiplied by 96.
	
	return ((double)dpiY) / 96.0;
}


double UiProvider::uiLengthToDips(const UiLength& uiLength, double uiScaleFactor)
{
	if(uiLength.unit==UiLength::sem)
		return uiLength.value * _semSizeDips;

	else if(uiLength.unit==UiLength::dip)
	{
        return uiLength.value;
    }
   
	else
		throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
}

Margin UiProvider::uiMarginToDipMargin(const UiMargin& margin, double uiScaleFactor)
{
	return Margin(
		uiLengthToDips(margin.top, uiScaleFactor),
		uiLengthToDips(margin.right, uiScaleFactor),
		uiLengthToDips(margin.bottom, uiScaleFactor),
		uiLengthToDips(margin.left, uiScaleFactor) );	
}

}
}

