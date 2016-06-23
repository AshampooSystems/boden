#include <bdn/init.h>
#include <bdn/GlobalMessageWindow.h>

#include <bdn/log.h>

namespace bdn
{


GlobalMessageWindow::GlobalMessageWindow()
	: MessageWindowBase("bdn::GlobalMessageWindow")
{
}
	
void GlobalMessageWindow::postCall(ISimpleCallable* pCallable)
{
	pCallable->addRef();
	::PostMessage( getHwnd(), MessageCall, 0, reinterpret_cast<LPARAM>(pCallable) );
}

void GlobalMessageWindow::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
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
			// log and ignore exceptions
			logError(e, "Exception thrown by ISimpleCallable::call during GlobalMessageWindow MessageCall handling. Ignoring.");
		}

		pCallable->releaseRef();

		context.setResult(0, false);
	}
}



}

