#import <bdn/foundationkit/AttributedString.hh>
#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/ios/ButtonCore.hh>
#import <bdn/ios/UIUtil.hh>

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

        label.onChange() += [=](auto &property) { textChanged(property.get()); };
        imageURL.onChange() += [=](auto &property) { imageChanged(property.get()); };
    }

    ButtonCore::~ButtonCore()
    {
        [_button removeTarget:_clickManager action:nil forControlEvents:UIControlEventTouchUpInside];
    }

    UIButton *ButtonCore::getUIButton() { return _button; }

    void ButtonCore::handleClick() { _clickCallback.fire(); }

    float ButtonCore::calculateBaseline(Size forSize) const
    {
        auto bounds = CGRectMake(0, 0, forSize.width, forSize.height);
        auto titleBox = [_button titleRectForContentRect:bounds];

        auto labelOffset = titleBox.origin.y;
        auto buttonBaseline = labelOffset + _button.titleLabel.font.ascender;

        auto scaleFactor = _button.titleLabel.contentScaleFactor;
        auto scaledBaseline = buttonBaseline * scaleFactor;

        buttonBaseline = ceil(scaledBaseline) / scaleFactor;

        return static_cast<float>(buttonBaseline);
    }

    float ButtonCore::pointScaleFactor() const { return _button.titleLabel.contentScaleFactor; }

    void ButtonCore::textChanged(const Text &text)
    {
        std::visit(
            [uiButton = this->_button](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, String>) {
                    [uiButton setTitle:fk::stringToNSString(arg) forState:UIControlStateNormal];
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto fkAttrString = std::dynamic_pointer_cast<bdn::fk::AttributedString>(arg)) {
                        [uiButton setAttributedTitle:fkAttrString->nsAttributedString() forState:UIControlStateNormal];
                    }
                }
            },
            text);

        markDirty();
    }

    void ButtonCore::imageChanged(const String &url)
    {
        UIButton *button = ((UIButton *)this->uiView());
        [button setImage:nullptr forState:UIControlStateNormal];

        bool imageChangedImmediately = false;

        if (!url.empty()) {
            imageChangedImmediately = imageFromUrl(url, [=](auto image) {
                [button setImage:image forState:UIControlStateNormal];

                scheduleLayout();
                markDirty();
            });
        }

        if (!imageChangedImmediately) {
            scheduleLayout();
            markDirty();
        }
    }
}
