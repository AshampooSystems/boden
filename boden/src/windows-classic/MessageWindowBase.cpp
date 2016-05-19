#include <bdn/init.h>
#include <bdn/MessageWindowBase.h>

namespace bdn
{


MessageWindowBase::MessageWindowBase(const String& windowName)
{
	String className = MessageWindowClass::get()->getName();

	createBase(className, windowName, WS_POPUP, 0, NULL );
}





}

