#import <Cocoa/Cocoa.h>
#import <bdn/mac/ButtonCore.hh>

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
        label.onChange() += [=](auto &property) {
            NSString *macLabel = fk::stringToNSString(label);
            [(NSButton *)nsView() setTitle:macLabel];
        };
    }

    ButtonCore::~ButtonCore() { _clickManager.buttonCore = std::weak_ptr<ButtonCore>(); }

    Size ButtonCore::sizeForSpace(Size availableSpace) const
    {
        auto s = ((NSButton *)nsView()).intrinsicContentSize;

        return macSizeToSize(s);
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
}
