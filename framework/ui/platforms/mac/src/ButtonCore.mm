#import <Cocoa/Cocoa.h>
#import <bdn/foundationkit/AttributedString.hh>
#import <bdn/mac/ButtonCore.hh>
#import <bdn/mac/UIUtil.hh>

@interface BdnButtonClickManager : NSObject
@property(nonatomic, assign) std::weak_ptr<bdn::ui::mac::ButtonCore> buttonCore;
@end

@implementation BdnButtonClickManager

- (void)clicked
{
    if (auto buttonCore = _buttonCore.lock()) {
        buttonCore->handleClick();
    }
}

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(Button, bdn::ui::mac::ButtonCore, Button)
}

namespace bdn::ui::mac
{
    NSButton *ButtonCore::_createNsButton()
    {
        NSButton *button = [[NSButton alloc] init];

        button.title = @"";

        [button setButtonType:NSButtonTypeMomentaryLight];
        [button setBezelStyle:NSBezelStyleRounded];

        return button;
    }

    ButtonCore::ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : mac::ViewCore(viewCoreFactory, _createNsButton()), _currBezelStyle(NSBezelStyleRounded)
    {}

    void ButtonCore::init()
    {
        mac::ViewCore::init();

        _clickManager = [[BdnButtonClickManager alloc] init];
        _clickManager.buttonCore = shared_from_this<ButtonCore>();
        [(NSButton *)nsView() setTarget:_clickManager];
        [(NSButton *)nsView() setAction:@selector(clicked)];

        _heightWithRoundedBezelStyle = (int)macSizeToSize(nsView().fittingSize).height;

        geometry.onChange() += [=](auto) { _updateBezelStyle(); };
        label.onChange() += [=](auto &property) { updateText(property.get()); };
        imageURL.onChange() += [=](auto &property) { updateImage(property.get()); };
    }

    ButtonCore::~ButtonCore() { _clickManager.buttonCore = std::weak_ptr<ButtonCore>(); }

    Size ButtonCore::sizeForSpace(Size availableSpace) const
    {
        Size s = ViewCore::sizeForSpace(availableSpace);
        auto insets = ((NSButton *)nsView()).alignmentRectInsets;

        s.width -= insets.left + insets.right;
        s.height -= insets.top + insets.bottom;

        return s;
    }

    float ButtonCore::calculateBaseline(Size forSize, bool forIndicator) const
    {
        auto baseline = nsView().firstBaselineOffsetFromTop;

        if (forIndicator) {
            baseline += ((NSButton *)nsView()).alignmentRectInsets.top;
        }

        return static_cast<float>(baseline);
    }

    void ButtonCore::frameChanged() {}

    void ButtonCore::setFrame(Rect r)
    {
        NSRect frame = rectToMacRect(r, -1);

        auto insets = ((NSButton *)nsView()).alignmentRectInsets;

        frame.origin.x -= insets.left;
        frame.origin.y -= insets.top;
        frame.size.width += insets.left + insets.right;
        frame.size.height += insets.top + insets.bottom;

        [nsView() setFrameOrigin:frame.origin];
        [nsView() setFrameSize:frame.size];
    }

    void ButtonCore::handleClick() { _clickCallback.fire(); }

    void ButtonCore::_updateBezelStyle()
    {
        // the "normal" button (NSRoundedBezelStyle) has a fixed height.
        // If we want a button that is higher then we have to use another
        // bezel style, which will look somewhat "non-standard" because it
        // has different highlights.

        // we switch to the non-standard bezel style when our height
        // "significantly" exceeds the fixed height we get with the standard
        // bezel.
        NSBezelStyle bezelStyle;

        if (geometry->height > _heightWithRoundedBezelStyle * 1.1) {
            bezelStyle = NSBezelStyleRegularSquare;
        } else {
            bezelStyle = NSBezelStyleRounded;
        }

        if (bezelStyle != _currBezelStyle) {
            [(NSButton *)nsView() setBezelStyle:bezelStyle];
        }
    }

    void ButtonCore::updateText(const Text &text)
    {
        std::visit(
            [nsButton = (NSButton *)this->nsView()](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, String>) {
                    nsButton.title = fk::stringToNSString(arg);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto fkAttrString = std::dynamic_pointer_cast<bdn::fk::AttributedString>(arg)) {
                        nsButton.attributedTitle = fkAttrString->nsAttributedString();
                    }
                }
            },
            text);

        scheduleLayout();
        markDirty();
    }

    void ButtonCore::updateImage(const String &url)
    {
        auto btn = (NSButton *)this->nsView();
        btn.image = nullptr;
        btn.imageScaling = NSImageScaleProportionallyUpOrDown;

        bool imageChangedImmediately = false;

        if (!url.empty()) {
            imageChangedImmediately = imageFromUrl(url, [=](auto img) {
                btn.image = img;
                this->scheduleLayout();
                this->markDirty();
            });
        }

        if (!imageChangedImmediately) {
            this->scheduleLayout();
            this->markDirty();
        }
    }
}
