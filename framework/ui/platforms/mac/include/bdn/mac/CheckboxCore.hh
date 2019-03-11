#pragma once

#include <bdn/Checkbox.h>
#include <bdn/CheckboxCore.h>
#include <bdn/SwitchCore.h>

#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/ui_util.hh>
#import <bdn/mac/util.hh>

namespace bdn::mac
{
    class CheckboxCore : public ViewCore, virtual public bdn::CheckboxCore
    {
      private:
        static NSButton *createNSButton()
        {
            NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
            [button setButtonType:NSButtonTypeSwitch];
            return button;
        }

      public:
        CheckboxCore() : ViewCore(createNSButton())
        {
            _nsButton = (NSButton *)nsView();

            _nsButton.allowsMixedState = true;

            label.onChange() += [=](auto va) {
                NSString *macLabel = stringToNSString(label);
                [_nsButton setTitle:macLabel];
            };

            state.onChange() += [=](auto va) {
                if (state == TriState::mixed) {
                    _nsButton.allowsMixedState = true;
                }
                _nsButton.state = triStateToNSControlStateValue(state);
            };
        }

        void handleClick()
        {
            if (_nsButton.state == NSControlStateValueOff) {
                _nsButton.allowsMixedState = false;
            } else {
                _nsButton.allowsMixedState = true;
            }
            state = nsControlStateValueToTriState(_nsButton.state);
            _clickCallback.fire();
        }

      private:
        NSButton *_nsButton;
    };
}
