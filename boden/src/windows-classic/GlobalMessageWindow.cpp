#include <bdn/init.h>
#include <bdn/GlobalMessageWindow.h>

namespace bdn
{


GlobalMessageWindow::GlobalMessageWindow()
	: MessageWindow("bdn::GlobalMessageWindow")
{
}
	
void GlobalMessageWindow::postCall(ISimpleCallable* pCallable)
{
	pCallable->addRef();
	::PostMessage(_windowHandle, MessageCall, 0, reinterpret_cast<LPARAM>(pCallable) );
}

LRESULT GlobalMessageWindow::windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==MessageCall)
	{
		ISimpleCallable* pCallable = reinterpret_cast<ISimpleCallable*>(lParam);

		try
		{
			pCallable->call();
		}
		catch(std::exception& e)
		{
			// ignore exceptions
		}

		pCallable->release();

		return 0;
	}

	return MessageWindow::windowProc(windowHandle, message, wParam, lParam);
}



}

