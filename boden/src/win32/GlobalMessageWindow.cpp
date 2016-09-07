#include <bdn/init.h>
#include <bdn/win32/GlobalMessageWindow.h>

#include <bdn/log.h>

#include <bdn/win32/win32Error.h>
#include <bdn/Thread.h>

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( GlobalMessageWindow, GlobalMessageWindow::get );

GlobalMessageWindow::GlobalMessageWindow()
	: RequireNewAlloc<MessageWindowBase, GlobalMessageWindow>("bdn::GlobalMessageWindow")
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
}

