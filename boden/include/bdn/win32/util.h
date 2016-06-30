#ifndef BDN_WIN32_util_H_
#define BDN_WIN32_util_H_

#include <bdn/Rect.h>

#include <Windows.h>

#include <vector>

namespace bdn
{
namespace win32
{
	

inline Rect win32RectToRect(const RECT& rect)
{
	return Rect( rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
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