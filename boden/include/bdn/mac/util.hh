#ifndef BDN_MAC_util_HH_
#define BDN_MAC_util_HH_


namespace bdn
{
namespace mac
{


inline Rect macRectToRect(const NSRect& rect)
{
	return Rect(
			std::lround(rect.origin.x),
			std::lround(rect.origin.y),
			std::lround(rect.size.width),
            std::lround(rect.size.height) );
}

inline NSRect rectToMacRect(const Rect& rect)
{
    NSRect macRect;
    
    macRect.origin.x = rect.x;
    macRect.origin.y = rect.y;
    macRect.size.width = rect.width;
    macRect.size.height = rect.height;
    
    return macRect;
}


inline Size macSizeToSize(const NSSize& macSize)
{
	return Size( macSize.width,
				 macSize.height );
}


inline NSString* stringToMacString(const String& val)
{
    return [NSString stringWithCString: val.asUtf8Ptr()
                     encoding: NSUTF8StringEncoding];
}


inline String macStringToString(NSString* nsString)
{
    return [nsString cStringUsingEncoding:NSUTF8StringEncoding];
}

		
}
}


#endif


