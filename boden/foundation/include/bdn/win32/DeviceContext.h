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


    /** Returns the size of the specified text in DIP units (see \ref dip.md).
    
        This function handles linebreaks correctly (i.e. it returns the size
        of multiple lines of text if the string contains linebreaks)

		If wrapWidth is a finite value then the text size is calculated for the case
		in which the text wraps at the specified width (in pixels).
		The wrapping occurs on word boundaries. If a word is wider than wrapWidth
		then the returned text size can be bigger than wrapWidth.

        wrapWidth can also be std::numeric_limits<double>::initity(), NaN
        or Size::componentNone(),  in which case no wrapping will be done.
    */
    Size getTextSize(const String& text, double wrapWidth = std::numeric_limits<double>::infinity() )
    {
        // GetTextExtentPoint32W ignores linebreak and also does not
        // provide any way to calculate the height of multiple lines of text. So we use
        // DrawText instead, which also has a size calculation function.

        const std::wstring& wideText = text.asWide();

        RECT rect{0, 0, 0, 0};

		UINT flags = DT_CALCRECT;

        // convert to device independent pixels
        int     dpi = ::GetDeviceCaps( _handle, LOGPIXELSX);
        double  scaleFactor = dpi / 96.0;
        

		if( std::isfinite(wrapWidth) )
		{
            if(wrapWidth<0)
                wrapWidth=0;

            // we round DOWN to the next pixel here. It is to be expected that the wrapped
            // width might be less than the specified wrap width (because wrapping always only
            // occurs on word or character boundaries). However, the wrap width should only
            // be exceeded if there is a word that is too wide to fit. We should not
            // exceed because of rounding.
			rect.right = (long)std::floor(wrapWidth * scaleFactor);
            
            // special case: Windows will not return a valid size if the target width is zero.
            // In that case we specify 1 instance.
            if(rect.right==0)
                rect.right=1;

			flags |= DT_WORDBREAK;
		}
		else
		{
			// DrawText uses the input width of rect as a measure for the available width
			// (so that it can do line-breaking).
			// Since we want unlimited width, we set it to a very large size
			rect.right = 0x7fffffff;
		}

        int result = ::DrawText(_handle, wideText.c_str(), (int)wideText.length(), &rect, flags);
        if(result==0)
        {
            BDN_WIN32_throwLastError( ErrorFields().add("func", "DrawText")
                                                    .add("action", "DeviceContext::getTextSize")
                                                    .add("text", text) );
        }

        return Size( rect.right / scaleFactor, rect.bottom / scaleFactor);
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

