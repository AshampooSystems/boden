#ifndef BDN_Window_H_
#define BDN_Window_H_

#include <Windows.h>

namespace bdn
{

class Window : public Base
{
public:


	/** Static function that returns the text of the specified window
		(like the Windows function GetWindowText)*/
	static String getWindowText(HWND windowHandle);
	

};

}

#endif

