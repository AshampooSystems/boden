#ifndef BDN_WIN32_Win32UiProvider_H_
#define BDN_WIN32_Win32UiProvider_H_

#include <bdn/IUiProvider.h>
#include <bdn/UiMargin.h>
#include <bdn/Margin.h>

#include <bdn/View.h>

#include <bdn/win32/Font.h>

#include <Windows.h>

namespace bdn
{
namespace win32
{
	

/** UI provider that creates classic Windows desktop UI components using the Win32 API.
	Note that Win32 is also the name of the Windows API on 64 Bit Windows,
	so this works on 32 and 64 bit systems.
*/
class Win32UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
	Win32UiProvider();

	
	String getName() const
	{
		return "win32";
	}


	P<IViewCore> createViewCore(const String& coreTypeName, View* pView);


	/** Returns the Windows font to use for the specified view and the specified
		UI scale factor.		
		*/
	P<Font> getFontForView(View* pView, double uiScaleFactor);


	/** Returns the UI scale factor for the screen that the specified window is on.*/
	double getUiScaleFactorForTopLevelWindow(HWND window);


	/** Returns the size of one sem for the specified UI scale factor.
		(also see UiLength::sem).
		
		uiScaleFactor is a value that depends on the screen that the top level window is on.
		It can be obtained via getUiScaleFactorForWindowScreen().*/
	double getSemSizeForUiScaleFactor(double uiScaleFactor);


	/** Converts the specified Ui length to pixels.
		uiScaleFactor is a value that depends on the screen that the top level window is on.
		It can be obtained via getUiScaleFactorForWindowScreen().*/
	int uiLengthToPixels(const UiLength& uiLength, double uiScaleFactor);
	

	/** Converts a UiMargin object to a pixel-based margin object.
		uiScaleFactor is a value that depends on the screen that the top level window is on.
		It can be obtained via getUiScaleFactorForWindowScreen().*/
	Margin uiMarginToPixelMargin(const UiMargin& margin, double uiScaleFactor);



	static P<Win32UiProvider> get()
	{
		static SafeInit<Win32UiProvider> init;

		return init.get();
	}

protected:
	struct FontSpec
	{
		FontSpec()
		{
			uiScaleFactor=1;
		}

		bool operator<(const FontSpec& spec) const
		{
			return uiScaleFactor < spec.uiScaleFactor;
		}

		double uiScaleFactor;		
	};

	P<Font> createFont(const FontSpec& spec);


	LOGFONTW					_defaultUiFontInfo;
	P<Font>						_pDefaultUiFont;

	double						_semSizeAtScaleFactor1;

	std::map< FontSpec, P<Font> > _fontMap;	
};

}
}

#endif