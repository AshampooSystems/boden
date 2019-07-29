#include <bdn/mac/CheckboxCore.hh>

@interface BdnCheckboxClickManager : NSObject
@property(nonatomic, assign) std::weak_ptr<bdn::ui::mac::CheckboxCore> checkboxCore;
@end

@implementation BdnCheckboxClickManager

- (void)clicked
{
    if (auto core = self.checkboxCore.lock()) {
        core->handleClick();
    }
}

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(Checkbox, bdn::ui::mac::CheckboxCore, Checkbox)
}

namespace bdn::ui::mac
{
    NSButton *CheckboxCore::createNSButton()
    {
        NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        [button setButtonType:NSButtonTypeSwitch];
        return button;
    }

    CheckboxCore::CheckboxCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : mac::ViewCore(viewCoreFactory, createNSButton())
    {}

    void CheckboxCore::init()
    {
        _nsButton = (NSButton *)nsView();
        _nsButton.title = @"";
        _nsButton.allowsMixedState = static_cast<BOOL>(false);

        _clickManager = [[BdnCheckboxClickManager alloc] init];
        _clickManager.checkboxCore = shared_from_this<CheckboxCore>();
        [(NSButton *)nsView() setTarget:_clickManager];
        [(NSButton *)nsView() setAction:@selector(clicked)];

        label.onChange() += [=](auto &property) {
            NSString *macLabel = fk::stringToNSString(label);
            [_nsButton setTitle:macLabel];
        };

        state.onChange() += [=](auto &property) {
            if (state == TriState::Mixed) {
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
