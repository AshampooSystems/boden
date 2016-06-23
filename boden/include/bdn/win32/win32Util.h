#ifndef BDN_WIN32_win32Util_H_
#define BDN_WIN32_win32Util_H_


namespace bdn
{
namespace
{
	

Rect win32RectToRect(const RECT& rect)
{
	return Rect( rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
}

}
}

#endif