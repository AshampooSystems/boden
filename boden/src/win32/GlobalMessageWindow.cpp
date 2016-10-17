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

GlobalMessageWindow::~GlobalMessageWindow()
{
    destroy();
}
	
void GlobalMessageWindow::postCall(ISimpleCallable* pCallable)
{
	pCallable->addRef();
	::PostMessage( getHwnd(), MessageCall, 0, reinterpret_cast<LPARAM>(pCallable) );
}



void GlobalMessageWindow::callOnceWhenIdle( ISimpleCallable* pCallable )
{
    {
        MutexLock lock( _idleMutex );
    
        _callOnceWhenIdleList.push_back( pCallable );
    }       

    // we may currently be in an idle phase. If that is the case then we
    // need to break it with a dummy message and cause a new idle phase to be begun.
    ::PostMessage( getHwnd(), WM_NULL, 0, 0);
}

void GlobalMessageWindow::notifyIdleBegun()
{
    std::list< P<ISimpleCallable> > toCallList;

    {
        MutexLock lock( _idleMutex );

        toCallList = _callOnceWhenIdleList;
        _callOnceWhenIdleList.clear();
    }

    // we want each handler to be released directly after it is called.
    // So we remove each item from the list after the call.
    while(!toCallList.empty())
    {
        P<ISimpleCallable>& pCallable = toCallList.front();

        try
		{
			pCallable->call();
		}
		catch(std::exception& e)
		{
			// log and ignore exceptions
			logError(e, "Exception thrown by ISimpleCallable::call during GlobalMessageWindow::notifyIdleBegun. Ignoring.");
		}

        
        toCallList.pop_front();
    }
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

