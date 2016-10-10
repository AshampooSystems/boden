#ifndef BDN_WIN32_util_H_
#define BDN_WIN32_util_H_

#include <bdn/Rect.h>

#include <Windows.h>

#include <vector>

namespace bdn
{
namespace win32
{
	

/** Converts a point in DIP coordinates to a win32 POINT structure with pixel coordinates.
    
    scaleFactor is the UI scale factor for the UI (i.e. the factor to convert from DIPs to pixels.

    The functions rounds the coordinates to the NEAREST pixel.
    
    */
inline POINT pointToWin32Point(const Point& point, double scaleFactor)
{
    POINT winPoint {(long)std::lround(point.x * scaleFactor),
                    (long)std::lround(point.y * scaleFactor) };

    return winPoint;
}



/** Converts a size in DIPs to a win32 SIZE in pixels.
    
    scaleFactor is the UI scale factor for the UI (i.e. the factor to convert from DIPs to pixels.

    The functions rounds the sizes to the NEAREST pixel amount.    
    */
inline SIZE sizeToWin32Size(const Size& size, double scaleFactor)
{
    SIZE winSize {(long)std::lround(size.width * scaleFactor),
                  (long)std::lround(size.height * scaleFactor) };

    return winSize;
}


/** Converts a rect in DIP coordinates to a win32 RECT structure with pixel coordinates.
    
    scaleFactor is the UI scale factor for the UI (i.e. the factor to convert from DIPs to pixels.

    The functions rounds the coordinates to the NEAREST pixel amount.
    */
inline RECT rectToWin32Rect(const Rect& rect, double scaleFactor)
{
    POINT winPos = pointToWin32Point( rect.getPosition(), scaleFactor );
    SIZE  winSize = sizeToWin32Size( rect.getSize(), scaleFactor );

    RECT winRect{
        winPos.x,
        winPos.y,
        winPos.x + winSize.cx,
        winPos.y+winSize.cy };

    return winRect;
}


/** Converts a win32 RECT structure with pixel coordinates to a Rect with DIP coordinates.

    scaleFactor is the UI scale factor for the UI (i.e. the factor to convert from DIPs to pixels.
    */
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