#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/win32/GlobalMessageWindow.h>

namespace bdn
{
	

void CallFromMainThreadBase_::dispatch()
{
	win32::GlobalMessageWindow::get()->postCall(this);
}


}

