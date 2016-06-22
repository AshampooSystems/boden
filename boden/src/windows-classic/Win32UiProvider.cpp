#include <bdn/init.h>
#include <bdn/Win32UiProvider.h>

#include <bdn/ButtonCore.h>
#include <bdn/WindowCore.h>
#include <bdn/ContainerViewCore.h>
#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/sysError.h>

#include <ShellScalingAPI.h>

namespace bdn
{

P<IUiProvider> getPlatformUiProvider()
{
	return Win32UiProvider::get();
}




Win32UiProvider::Win32UiProvider()
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
		BDN_throwLastSysError( ErrorFields().add("func", "SystemParametersInfo(SPI_GETNONCLIENTMETRICS)")
											.add("action", "Win32UiProvider constructor") );
	}

	_defaultUiFontInfo = metrics.lfMessageFont;

	_pDefaultUiFont = newObj<Font>(_defaultUiFontInfo);

	double fontSize = _pDefaultUiFont->getSizePixels();

	// now we have the font size on a 96 DPI screen, which is the base DPI
	// that windows assigns to UI elements when the UI scaling factor is 1.
	_semSizeAtScaleFactor1 = fontSize;

	// add the font to our map
	FontSpec spec;
	_fontMap[spec] = _pDefaultUiFont;
}


P<Font> Win32UiProvider::getFontForView(View* pView, double uiScaleFactor)
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

P<Font> Win32UiProvider::createFont(const FontSpec& spec)
{
	LOGFONT info = _defaultUiFontInfo;

	info.lfHeight = std::lround( ((double)info.lfHeight)*spec.uiScaleFactor );

	return newObj<Font>(info);
}


P<IViewCore> Win32UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
	if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
		return newObj<ContainerViewCore>( cast<ContainerView>(pView) );

	else if(coreTypeName == Button::getButtonCoreTypeName() )
		return newObj<ButtonCore>( cast<Button>(pView) );

	else if(coreTypeName == Window::getWindowCoreTypeName() )
		return newObj<WindowCore>( cast<Window>(pView) );

	else	
		throw ViewCoreTypeNotSupportedError(coreTypeName);
}




double Win32UiProvider::getUiScaleFactorForTopLevelWindow(HWND window)
{	
	HMONITOR	hMonitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

	UINT dpiX = 0;
	UINT dpiY = 0;
	::GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX ,&dpiY);

	// note that the DPI we get here is not a real physical DPI. Rather it is a more complicated
	// value that takes into account the device type and even user settings.
	// It is basically the UI scale factor that Windows wants us to use, multiplied by 96.
	
	return ((double)dpiY) / 96.0;
}



double Win32UiProvider::getSemSizeForUiScaleFactor(double uiScaleFactor)
{
	return _semSizeAtScaleFactor1 * uiScaleFactor;
}

int Win32UiProvider::uiLengthToPixels(const UiLength& uiLength, double uiScaleFactor)
{
	if(uiLength.unit==UiLength::sem)
		return std::lround( uiLength.value * getSemSizeForUiScaleFactor(uiScaleFactor) );

	else if(uiLength.unit==UiLength::pixel96)
	{
		// See UiLength documentation for more information about the pixel96 unit
		// and why this is correct.
		return std::lround( uiLength.value * uiScaleFactor );
	}

	else
		return std::lround( uiLength.value );
}

Margin Win32UiProvider::uiMarginToPixelMargin(const UiMargin& margin, double uiScaleFactor)
{
	return Margin(
		uiLengthToPixels(margin.top, uiScaleFactor),
		uiLengthToPixels(margin.right, uiScaleFactor),
		uiLengthToPixels(margin.bottom, uiScaleFactor),
		uiLengthToPixels(margin.left, uiScaleFactor) );	
}


}