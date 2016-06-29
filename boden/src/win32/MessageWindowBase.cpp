#include <bdn/init.h>
#include <bdn/win32/MessageWindowBase.h>

namespace bdn
{
namespace win32
{


BDN_SAFE_STATIC_IMPL( MessageWindowBase, MessageWindowBase::get );

MessageWindowBase::MessageWindowBase(const String& windowName)
	: Win32Window(	MessageWindowClass::get()->getName(),
					windowName,
					WS_POPUP,
					0,
					NULL )
{
}


}
}

