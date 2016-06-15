#include <bdn/init.h>
#include <bdn/MessageWindowBase.h>

namespace bdn
{


MessageWindowBase::MessageWindowBase(const String& windowName)
	: Win32Window(	MessageWindowClass::get()->getName(),
					windowName,
					WS_POPUP,
					0,
					NULL )
{
}



}

