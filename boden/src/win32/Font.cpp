#include <bdn/init.h>
#include <bdn/win32/Font.h>

#include <bdn/sysError.h>

namespace bdn
{
namespace win32
{
	

Font::Font(const LOGFONT& fontInfo)
{
	_handle = ::CreateFontIndirect(&fontInfo);
	if(_handle==NULL)
	{
		BDN_throwLastSysError( ErrorFields().add("func", "CreateFontIndirect")
											.add("action", "Font constructor") );
	}
}

Font::~Font()
{
	if(_handle!=NULL)
		::DeleteObject(_handle);
}


TEXTMETRIC Font::getMetrics() const
{
	TEXTMETRIC metrics = {0};

	HDC screenContext = ::GetDC( NULL );
	if(screenContext==NULL)
	{
		BDN_throwLastSysError( ErrorFields().add("func", "GetDC")
											.add("action", "Font::getSizePixels") );
	}

	::SelectObject(screenContext, _handle);
	
	if(!::GetTextMetricsW(screenContext, &metrics))
	{
		BDN_throwLastSysError( ErrorFields().add("func", "GetTextMetricsW")
											.add("action", "Font::getSizePixels") );		
	}

	::ReleaseDC(NULL, screenContext);	

	return metrics;
}

double Font::getSizePixels() const
{
	TEXTMETRIC metrics = getMetrics();

	// the metrics are in logical units.
	// However, assuming that the DCs map mode is MM_TEXT (the default)
	// then the factor between logical <-> device is 1:1.

	return metrics.tmHeight;
}


}
}