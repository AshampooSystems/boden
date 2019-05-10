#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ButtonCore.hh>

@interface BodenUIButton : UIButton <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ButtonCore> core;
@end

@implementation BodenUIButton

- (void)setViewCore:(std::weak_ptr<bdn::ui::ios::ViewCore>)viewCore
{
    self.core = std::dynamic_pointer_cast<bdn::ui::ios::ButtonCore>(viewCore.lock());
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto core = self.core.lock()) {
        core->frameChanged();
    }
}

- (void)clicked
{
    if (auto core = self.core.lock()) {
        core->handleClick();
    }
}

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(Button, bdn::ui::ios::ButtonCore, Button)
}

namespace bdn::ui::ios
{
    BodenUIButton *_createUIButton() { return [BodenUIButton buttonWithType:UIButtonTypeSystem]; }

    ButtonCore::ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, _createUIButton())
    {
        _button = (UIButton *)uiView();
        [_button addTarget:_button action:@selector(clicked) forControlEvents:UIControlEventTouchUpInside];

        label.onChange() +=
            [=](auto &property) { [_button setTitle:fk::stringToNSString(label) forState:UIControlStateNormal]; };
    }

    ButtonCore::~ButtonCore()
    {
        [_button removeTarget:_clickManager action:nil forControlEvents:UIControlEventTouchUpInside];
    }

    UIButton *ButtonCore::getUIButton() { return _button; }

    void ButtonCore::handleClick() { _clickCallback.fire(); }
}
