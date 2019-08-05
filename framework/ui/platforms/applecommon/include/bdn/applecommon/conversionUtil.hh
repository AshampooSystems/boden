#pragma once

#include <bdn/platform.h>
#include <bdn/ui/TextOverflow.h>

#ifdef BDN_PLATFORM_OSX
#import <AppKit/AppKit.h>
#else
#import <UIKit/UIKit.h>
#endif

namespace bdn::ui::applecommon
{
    inline NSLineBreakMode textOverflowToLineBreakMode(TextOverflow overflow)
    {
        switch (overflow) {
        case TextOverflow::EllipsisHead:
            return NSLineBreakByTruncatingHead;
        case TextOverflow::EllipsisTail:
            return NSLineBreakByTruncatingTail;
        case TextOverflow::EllipsisMiddle:
            return NSLineBreakByTruncatingMiddle;
        case TextOverflow::Clip:
            return NSLineBreakByClipping;
        case TextOverflow::ClipWord:
            return NSLineBreakByWordWrapping;
        }

        return NSLineBreakByWordWrapping;
    }
}
