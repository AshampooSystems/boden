#include <bdn/init.h>
#include <bdn/WindowClass.h>

#include <bdn/sysError.h>

namespace bdn
{

WindowClass::WindowClass(const String& name, WNDPROC windowProc)
{
	_name = name;

	_info = {0};

	_info.cbSize = sizeof(WNDCLASSEX);
	_info.lpfnWndProc = windowProc;
	_info.lpszClassName = _name.asWidePtr();
	
	_registered = false;	
}

void WindowClass::ensureRegistered()
{
	if(!_registered)
	{
		if(::RegisterClassEx(&_info)==0)
		{
			BDN_throwLastSysError(
				ErrorFields().add("func", "RegisterClassEx")
							.add("name", _name) );
		}

		_registered = true;
	}
}


}


