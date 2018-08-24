#ifndef BDN_WIN32_WindowDeviceContext_H_
#define BDN_WIN32_WindowDeviceContext_H_

#include <bdn/win32/DeviceContext.h>

#include <Windows.h>

namespace bdn
{
namespace win32
{


/** A win32 device context for a window, as obtained via GetWindowDC.*/
class WindowDeviceContext : public DeviceContext
{
private:
    static HDC _createDC(HWND windowHandle)
    {
        HDC handle = ::GetWindowDC(windowHandle);
        if(handle==NULL)
        {
            BDN_WIN32_throwLastError( ErrorFields().add("func", "GetWindowDC")
                                                    .add("action", "WindowDeviceContext constructor") );
        }

        return handle;
    }

public:
	explicit WindowDeviceContext(HWND windowHandle)
        : DeviceContext( _createDC(windowHandle) )
    {
        _windowHandle = windowHandle;
    }

	~WindowDeviceContext()
    {
        ::ReleaseDC(_windowHandle, getHandle() );
    }

private:
    HWND _windowHandle;

};
	


}
}


#endif

