#include <bdn/init.h>
#include <bdn/win32/MessageWindowBase.h>

namespace bdn
{
namespace win32
{


BDN_SAFE_STATIC_IMPL( MessageWindowBase::MessageWindowClass, MessageWindowBase::MessageWindowClass::get );

MessageWindowBase::MessageWindowBase(const String& windowName)
	: Win32Window(	MessageWindowBase::MessageWindowClass::get().getName(),
					windowName,
					WS_POPUP,
					0,
					NULL )
{
}


}
}

