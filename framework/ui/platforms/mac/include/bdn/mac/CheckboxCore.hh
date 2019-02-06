#pragma once

#include <bdn/CheckboxCore.h>
#include <bdn/SwitchCore.h>
#include <bdn/Checkbox.h>

#import <bdn/mac/ButtonCoreBase.hh>
#import <bdn/mac/util.hh>

namespace bdn
{
    namespace mac
    {
        class CheckboxCore : public ButtonCoreBase, virtual public bdn::CheckboxCore
        {
          private:
            static NSButton *createNSButton()
            {
                NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                [button setButtonType:NSButtonTypeSwitch];
                return button;
            }

          public:
            CheckboxCore(std::shared_ptr<Checkbox> outer) : ButtonCoreBase(outer, createNSButton())
            {
                _nsButton = (NSButton *)getNSView();

                if (std::dynamic_pointer_cast<Checkbox>(outer))
                    _nsButton.allowsMixedState = true;

                setLabel(outer->label);
                setState(outer->state);
            }

            void setState(const TriState &state) override
            {
                if (state == TriState::mixed) {
                    // Explicitly allow for mixed state in NSButton when setting
                    // Checkbox::State::mixed programatically
                    _nsButton.allowsMixedState = true;
                }

                _nsButton.state = triStateToNSControlStateValue(state);
            }

            virtual void setLabel(const String &label) override { ButtonCoreBase::setLabel(label); }

            void generateClick()
            {
                std::shared_ptr<Checkbox> outer = std::dynamic_pointer_cast<Checkbox>(getOuterViewIfStillAttached());
                if (outer != nullptr) {
                    bdn::ClickEvent evt(outer);

                    // Observing NSButton's state via KVO does not work when
                    // the button's state is changed via user interaction. KVO
                    // works though when state is set programatically, which
                    // unfortunately is useless in the case that a user changes
                    // the button's state. This means we have to stick to the
                    // click event to propagate the state change to the
                    // framework. The state will be set before the onClick
                    // notification is posted.

                    // Prohibit setting mixed state via user interaction.
                    // When NSButton allows for mixed state, it also allows
                    // users to toggle the checkbox to mixed state by
                    // clicking (Off => Mixed => On). To prevent this
                    // behavior, we deactivate allowsMixedState when
                    // NSButton is switched to off state and reactivate it
                    // when it is set to on state.
                    if (_nsButton.state == NSControlStateValueOff) {
                        _nsButton.allowsMixedState = false;
                    } else {
                        _nsButton.allowsMixedState = true;
                    }

                    outer->state = nsControlStateValueToTriState(_nsButton.state);
                    outer->onClick().notify(evt);
                }
            }

          private:
            NSButton *_nsButton;
        };
    }
}
