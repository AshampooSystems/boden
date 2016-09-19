#ifndef BDN_WIN32_util_H_
#define BDN_WIN32_util_H_

#include <bdn/Rect.h>

#include <Windows.h>

#include <vector>

namespace bdn
{
namespace win32
{
	

inline RECT rectToWin32Rect(const Rect& rect, double scaleFactor)
{
    RECT winRect{   (long)std::lround(rect.x * scaleFactor),
                    (long)std::lround(rect.y * scaleFactor),
                    0,
                    0 };

    winRect.right = winRect.left + (long)std::ceil(rect.width * scaleFactor);
    winRect.bottom = winRect.top + (long)std::ceil(rect.height * scaleFactor);

    return winRect;
}


inline Rect win32RectToRect(const RECT& rect, double scaleFactor)
{
	return Rect( rect.left / scaleFactor,
                 rect.top / scaleFactor,
                 (rect.right-rect.left) / scaleFactor,
                 (rect.bottom-rect.top) / scaleFactor );
}


/** Parses a wide character commandline string (as returned by the win32 API function
	GetCommandLineW.
	Returns an array (std::vector) with the individual arguments. The first element in the array
	will be the executable name that was included in the commandline.	
	*/
std::vector<String> parseWin32CommandLine(const String& commandline);


}
}

#endif