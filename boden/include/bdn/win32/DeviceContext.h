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


    /** Returns the size of the specified text.
    
        This function handles linebreaks correctly (i.e. it returns the size
        of multiple lines of text if the string contains linebreaks)
    */
    Size getTextSize(const String& text)
    {
        // GetTextExtentPoint32W ignores linebreak and also does not
        // provide any way to calculate the height of multiple lines of text. So we use
        // DrawText instead, which also has a size calculation function.

        const std::wstring& wideText = text.asWide();

        RECT rect{0, 0, 0, 0};

        // DrawText uses the input width of rect as a measure for the available width
        // (so that it can do line-breaking).
        // Since we want unlimited width, we set it to a very large size
        rect.right = 0x7fffffff;

        int result = ::DrawText(_handle, wideText.c_str(), wideText.length(), &rect, DT_CALCRECT);
        if(result==0)
        {
            BDN_WIN32_throwLastError( ErrorFields().add("func", "DrawText")
                                                    .add("action", "DeviceContext::getTextSize")
                                                    .add("text", text) );
        }

        return Size( rect.right, rect.bottom);
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

