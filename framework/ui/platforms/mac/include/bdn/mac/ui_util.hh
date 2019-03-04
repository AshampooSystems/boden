#pragma once

#import <AppKit/AppKit.h>
#include <Availability.h>
#import <bdn/foundationkit/stringUtil.hh>

namespace bdn::mac
{
#if defined(MAC_OS_X_VERSION_10_13) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_13

    inline NSControlStateValue triStateToNSControlStateValue(TriState state)
        __OSX_AVAILABLE_STARTING(__MAC_10_13, __IPHONE_NA)
    {
        switch (state) {
        case TriState::on:
            return NSControlStateValueOn;
        case TriState::off:
            return NSControlStateValueOff;
        case TriState::mixed:
            return NSControlStateValueMixed;
        }
        return NSControlStateValueOff;
    }

    inline TriState nsControlStateValueToTriState(NSControlStateValue nsState)
        __OSX_AVAILABLE_STARTING(__MAC_10_13, __IPHONE_NA)
    {
        switch (nsState) {
        case NSControlStateValueOn:
            return TriState::on;
        case NSControlStateValueOff:
            return TriState::off;
        case NSControlStateValueMixed:
            return TriState::mixed;
        }
        return TriState::off;
    }

#else
    inline NSInteger triStateToNSControlStateValue(TriState state)
    {
        switch (state) {
        case TriState::on:
            return NSOnState;
        case TriState::off:
            return NSOffState;
        case TriState::mixed:
            return NSMixedState;
        }
        return NSOffState;
    }

    inline TriState nsControlStateValueToTriState(NSInteger state)
    {
        switch (state) {
        case NSOnState:
            return TriState::on;
        case NSOffState:
            return TriState::off;
        case NSMixedState:
            return TriState::mixed;
        }
        return TriState::off;
    }
#endif
}
