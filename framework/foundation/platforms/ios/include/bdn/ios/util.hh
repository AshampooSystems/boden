#pragma once

#import <CoreGraphics/CoreGraphics.h>
#import <Foundation/Foundation.h>

#include <bdn/Rect.h>

namespace bdn::ios
{ /** Converts an iOS rect to a bdn::Rect.
   */
    inline Rect iosRectToRect(const CGRect &rect)
    {
        // iOS also uses DIPs, so no conversion necessary

        return Rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
    }

    /** Converts a bdn::Rect to an iOS rect. */
    inline CGRect rectToIosRect(const Rect &rect)
    {
        // iOS also uses DIPs, so no conversion necessary
        CGRect iosRect;

        iosRect.origin.x = rect.x;
        iosRect.origin.y = rect.y;
        iosRect.size.width = rect.width;
        iosRect.size.height = rect.height;

        return iosRect;
    }

    inline Size iosSizeToSize(const CGSize &iosSize) { return Size(iosSize.width, iosSize.height); }

    inline CGSize sizeToIosSize(const Size &size)
    {
        CGSize iosSize;

        iosSize.width = size.width;
        iosSize.height = size.height;

        return iosSize;
    }

    inline Point iosPointToPoint(const CGPoint &iosPoint) { return Point(iosPoint.x, iosPoint.y); }

    inline CGPoint pointToIosPoint(const Point &p)
    {
        CGPoint iosPoint;

        iosPoint.x = p.x;
        iosPoint.y = p.y;

        return iosPoint;
    }
}
