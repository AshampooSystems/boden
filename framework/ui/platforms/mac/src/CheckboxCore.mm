#include <bdn/mac/CheckboxCore.hh>

namespace bdn::detail
{
    CORE_REGISTER(Checkbox, bdn::mac::CheckboxCore, Checkbox)
}

namespace bdn::mac
{
    NSButton *CheckboxCore::createNSButton()
    {
        NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        [button setButtonType:NSButtonTypeSwitch];
        return button;
    }

    CheckboxCore::CheckboxCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : bdn::mac::ViewCore(viewCoreFactory, createNSButton())
    {
        _nsButton = (NSButton *)nsView();

        _nsButton.allowsMixedState = static_cast<BOOL>(true);

        label.onChange() += [=](auto va) {
            NSString *macLabel = fk::stringToNSString(label);
            [_nsButton setTitle:macLabel];
        };

        state.onChange() += [=](auto va) {
            if (state == TriState::mixed) {
                _nsButton.allowsMixedState = static_cast<BOOL>(true);
            }
            _nsButton.state = triStateToNSControlStateValue(state);
        };
    }

    void CheckboxCore::handleClick()
    {
        if (_nsButton.state == NSControlStateValueOff) {
            _nsButton.allowsMixedState = static_cast<BOOL>(false);
        } else {
            _nsButton.allowsMixedState = static_cast<BOOL>(true);
        }
        state = nsControlStateValueToTriState(_nsButton.state);
        _clickCallback.fire();
    }
}
