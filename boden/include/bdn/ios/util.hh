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
    // iOS also uses DIPs, so no conversion necessary

	return Rect(
            rect.origin.x,
            rect.origin.y,
            rect.size.width,
            rect.size.height );
}



/** Converts a bdn::Rect to an iOS rect. */
inline CGRect rectToIosRect(const Rect& rect)
{
    // iOS also uses DIPs, so no conversion necessary
    CGRect iosRect;
    
    iosRect.origin.x = rect.x;
    iosRect.origin.y = rect.y;
    iosRect.size.width = rect.width;
    iosRect.size.height = rect.height;
    
    return iosRect;
}


inline Size iosSizeToSize(const CGSize& iosSize)
{
	return Size( iosSize.width,
				 iosSize.height );
}

inline CGSize sizeToIosSize(const Size& size)
{
    CGSize iosSize;
    
    iosSize.width = size.width;
    iosSize.height = size.height;
    
    return iosSize;
}

inline Point iosPointToPoint(const CGPoint& iosPoint)
{
    return Point( iosPoint.x,
                iosPoint.y );
}

inline CGPoint pointToIosPoint(const Point& p)
{
    CGPoint iosPoint;
    
    iosPoint.x = p.x;
    iosPoint.y = p.y;
    
    return iosPoint;
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


