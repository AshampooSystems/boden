#ifndef BDN_win32Util_H_
#define BDN_win32Util_H_


namespace bdn
{
	

Rect win32RectToRect(const RECT& rect)
{
	return Rect( rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
}


}

#endif