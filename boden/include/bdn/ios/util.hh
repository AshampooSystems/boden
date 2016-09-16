#ifndef BDN_IOS_util_HH_
#define BDN_IOS_util_HH_


namespace bdn
{
namespace ios
{


/** Converts an iOS rect to a bdn::Rect.
    */
inline Rect iosRectToRect(const CGRect& rect)
{
	return Rect(
			(int)std::lround(rect.origin.x),
            (int)std::lround(rect.origin.y),
			(int)std::ceil(rect.size.width),
            (int)std::ceil(rect.size.height) );
}



/** Converts a bdn::Rect to an iOS rect. */
inline CGRect rectToIosRect(const Rect& rect)
{
    CGRect iosRect;
    
    iosRect.origin.x = rect.x;
    iosRect.origin.y = rect.y;
    iosRect.size.width = rect.width;
    iosRect.size.height = rect.height;
    
    return iosRect;
}


inline Size iosSizeToSize(const CGSize& iosSize)
{
	return Size( std::ceil(iosSize.width),
				 std::ceil(iosSize.height) );
}

inline CGSize sizeToIosSize(const Size& size)
{
    CGSize iosSize;
    
    iosSize.width = size.width;
    iosSize.height = size.height;
    
    return iosSize;
}


inline NSString* stringToIosString(const String& val)
{
    return [NSString stringWithCString: val.asUtf8Ptr()
                     encoding: NSUTF8StringEncoding];
}


inline String iosStringToString(NSString* nsString)
{
    return [nsString cStringUsingEncoding:NSUTF8StringEncoding];
}

		
}
}


#endif


