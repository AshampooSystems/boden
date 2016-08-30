#ifndef BDN_WIN32_DeviceContext_H_
#define BDN_WIN32_DeviceContext_H_

#include <bdn/win32/Font.h>

#include <bdn/win32/win32Error.h>

#include <Windows.h>

namespace bdn
{
namespace win32
{


/** Base class for win32 device contexts (HDC).*/
class DeviceContext : public Base
{
public:
	explicit DeviceContext(HDC handle)
    {
        _handle = handle;
    }
    

    /** Selects a font into the device context.*/
    void setFont(const Font& font)
    {
        ::SelectObject( _handle, font.getHandle() );
    }

    Size getTextSize(const String& text)
    {
        SIZE textSize = {0};

        const std::wstring& textWide = text.asWide();

    	if(!::GetTextExtentPoint32W( _handle, textWide.c_str(), textWide.length(), &textSize))
        {
            BDN_WIN32_throwLastError( ErrorFields().add("func", "GetTextExtentPoint32W")
                                                   .add("action", "DeviceContext::getTextSize") );
        }

        return Size(textSize.cx, textSize.cy);
    }

    
    /** Returns the win32 HDC handle of the device context.*/
	HDC getHandle() const
	{
		return _handle;
	}

protected:
	HDC     _handle;
};
	


}
}


#endif

