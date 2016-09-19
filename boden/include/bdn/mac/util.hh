#ifndef BDN_MAC_util_HH_
#define BDN_MAC_util_HH_


namespace bdn
{
namespace mac
{


/** Converts a mac rect to a bdn::Rect.

    Mac UI objects often use an inverted coordinate space with the origin in
    the lower left corner, rather than the lower right.
    
    This function can convert from the inverted space to "normal" space with
    top left being the origin. To do that you must set coordinateSpaceHeightForFlipping
    to the full height of the coordinate space (i.e. the height of the area that the rect is
    in).
    
    If coordinateSpaceHeightForFlipping is -1 then no flipping is done.
    */
inline Rect macRectToRect(const NSRect& rect, int coordinateSpaceHeightForFlipping)
{
    XXX

    double y = rect.origin.y;
    
    if(coordinateSpaceHeightForFlipping!=-1)
    {
        // when we need to flip then we need to adjust for two things:
        // 1) the origin of the mac coordinate space is the bottom left
        // 2) the "position" of the rect refers to the lower left corner of
        //    the rect.
        
        // Adjust for 1)
        y = coordinateSpaceHeightForFlipping-y;
        
        // Adjust for 2)
        y -= rect.size.height;
    }

	return Rect(
			std::lround(rect.origin.x),
            std::lround(y),
			std::ceil(rect.size.width),
            std::ceil(rect.size.height) );
}



/** Converts a bdn::Rect to a mac rect.
     
    Mac UI objects often use an inverted coordinate space with the origin in
    the lower left corner, rather than the lower right.

    This function can convert from the "normal" space with
    top left being the origin to the inverted mac space.
    To do that you must set coordinateSpaceHeightForFlipping
    to the full height of the coordinate space (i.e. the height of the area that the rect is
    in).

    If coordinateSpaceHeightForFlipping is -1 then no flipping is done.
    */
inline NSRect rectToMacRect(const Rect& rect, int coordinateSpaceHeightForFlipping)
{
    XXX

    NSRect macRect;
    
    macRect.origin.x = rect.x;
    macRect.origin.y = rect.y;
    macRect.size.width = rect.width;
    macRect.size.height = rect.height;
    
    if(coordinateSpaceHeightForFlipping!=-1)
    {
        // seem acRectToRect for an explanation
        macRect.origin.y += macRect.size.height;
        macRect.origin.y = coordinateSpaceHeightForFlipping-macRect.origin.y;
    }
    
    return macRect;
}


inline Size macSizeToSize(const NSSize& macSize)
{
    XXX
	return Size( std::ceil(macSize.width),
				 std::ceil(macSize.height) );
}


inline NSSize sizeToMacSize(const Size& size)
{
    XXX

    NSSize macSize;
    macSize.width = size.width;
    macSize.height = size.height;
    
    return macSize;
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


