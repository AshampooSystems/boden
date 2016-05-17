#include <bdn/init.h>
#include <bdn/Window.h>


namespace bdn
{
	
	
String Window::getWindowText(HWND windowHandle)
{
	String result;

	int		prevLen = 0;
	while(true)
	{
		int len = ::GetWindowTextLength(windowHandle);

		// make sure that we use a bigger buffer in each iteration
		if(len < prevLen*2)
			len = prevLen*2;

		len += 10;					

		wchar_t* pBuf = new wchar_t[len+1];

		int actualLen = GetWindowText(windowHandle, pBuf, len);
		if( actualLen < len)
		{
			// success.
			pBuf[actualLen] = 0;

			result = pBuf;
			delete[] pBuf;

			break;
		}

		delete[] pBuf;

		// our buffer was not big enough (i.e. someone changed the text in between).
		// It is uncertain whether this can actually ever happen, but we handle it anyway.
		prevLen = len;
	}

	return result;
}

}

