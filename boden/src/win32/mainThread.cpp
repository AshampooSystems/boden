#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/GlobalMessageWindow.h>

namespace bdn
{
	

void CallFromMainThreadBase_::dispatch()
{
	GlobalMessageWindow::get()->postCall(this);
}


}

