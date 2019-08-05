#pragma once

#import <AppKit/AppKit.h>
#import <bdn/foundationkit/conversionUtil.hh>

#include <Availability.h>
#include <functional>

#include <bdn/Application.h>
#include <bdn/log.h>
#include <bdn/ui/TriState.h>

namespace bdn::ui::mac
{
    bool imageFromUrl(const std::string &url, std::function<void(NSImage *)> callback);

#if defined(MAC_OS_X_VERSION_10_13) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_13

    inline NSControlStateValue triStateToNSControlStateValue(TriState state)
        __OSX_AVAILABLE_STARTING(__MAC_10_13, __IPHONE_NA)
    {
        switch (state) {
        case TriState::On:
            return NSControlStateValueOn;
        case TriState::Off:
            return NSControlStateValueOff;
        case TriState::Mixed:
            return NSControlStateValueMixed;
        }
        return NSControlStateValueOff;
    }

    inline TriState nsControlStateValueToTriState(NSControlStateValue nsState)
        __OSX_AVAILABLE_STARTING(__MAC_10_13, __IPHONE_NA)
    {
        switch (nsState) {
        case NSControlStateValueOn:
            return TriState::On;
        case NSControlStateValueOff:
            return TriState::Off;
        case NSControlStateValueMixed:
            return TriState::Mixed;
        }
        return TriState::Off;
    }

#else
    inline NSInteger triStateToNSControlStateValue(TriState state)
    {
        switch (state) {
        case TriState::On:
            return NSOnState;
        case TriState::Off:
            return NSOffState;
        case TriState::Mixed:
            return NSMixedState;
        }
        return NSOffState;
    }

    inline TriState nsControlStateValueToTriState(NSInteger state)
    {
        switch (state) {
        case NSOnState:
            return TriState::On;
        case NSOffState:
            return TriState::Off;
        case NSMixedState:
            return TriState::Mixed;
        }
        return TriState::Off;
    }
#endif
}
