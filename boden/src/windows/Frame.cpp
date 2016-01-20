#include <bdn/init.h>
#include <bdn/Frame.h>
#include <bdn/Button.h>

namespace bdn
{

	LRESULT CALLBACK Frame::windowProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_CLOSE)
		{
			::PostQuitMessage(0);
		}
		else if (message == WM_COMMAND)
		{
			int code = HIWORD(wParam);
			int controlID = LOWORD(wParam);
			HWND hControl = (HWND)lParam;

			if (hControl != NULL)
			{
				Base*	pControl = (Base*)::GetWindowLongPtr(hControl, GWLP_USERDATA);
				Button* pButton = dynamic_cast<Button*>(pControl);
				if (pButton != nullptr)
					pButton->onClick();
			}
		}
		
		return ::CallWindowProc(DefWindowProc, hWindow, message, wParam, lParam);
	}


}

