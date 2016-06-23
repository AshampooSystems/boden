#include <bdn/init.h>
#include <bdn/win32/MessageWindowBase.h>

namespace bdn
{
namespace win32
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
}

