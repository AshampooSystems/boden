#import <Cocoa/Cocoa.h>
#import <bdn/mac/ButtonCore.hh>

@interface BdnButtonClickManager : NSObject
@property(nonatomic, assign) std::weak_ptr<bdn::mac::ButtonCore> buttonCore;
@end

@implementation BdnButtonClickManager

- (void)clicked
{
    if (auto buttonCore = _buttonCore.lock()) {
        buttonCore->handleClick();
    }
}

@end

namespace bdn::detail
{
    CORE_REGISTER(Button, bdn::mac::ButtonCore, Button)
}

namespace bdn::mac
{
    NSButton *ButtonCore::_createNsButton()
    {
        NSButton *button = [[NSButton alloc] init];

        [button setButtonType:NSButtonTypeMomentaryLight];
        [button setBezelStyle:NSBezelStyleRounded];

        return button;
    }

    ButtonCore::ButtonCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : bdn::mac::ViewCore(viewCoreFactory, _createNsButton()), _currBezelStyle(NSBezelStyleRounded)
    {}

    void ButtonCore::init()
    {
        bdn::mac::ViewCore::init();

        _clickManager = [[BdnButtonClickManager alloc] init];
        _clickManager.buttonCore = std::dynamic_pointer_cast<ButtonCore>(shared_from_this());
        [(NSButton *)nsView() setTarget:_clickManager];
        [(NSButton *)nsView() setAction:@selector(clicked)];

        _heightWithRoundedBezelStyle = (int)macSizeToSize(nsView().fittingSize).height;

        geometry.onChange() += [=](auto) { _updateBezelStyle(); };
        label.onChange() += [=](auto va) {
            NSString *macLabel = fk::stringToNSString(label);
            [(NSButton *)nsView() setTitle:macLabel];
        };
    }

    ButtonCore::~ButtonCore() { _clickManager.buttonCore = std::weak_ptr<ButtonCore>(); }

    Size ButtonCore::sizeForSpace(Size availableSpace) const
    {
        // the bezel style influences the fitting size. To get
        // consistent values here we have to ensure that we use the same
        // bezel style each time we calculate the size.

        NSBezelStyle bezelStyle = ((NSButton *)nsView()).bezelStyle;
        if (bezelStyle != NSBezelStyleRounded) {
            ((NSButton *)nsView()).bezelStyle = NSBezelStyleRounded;
        }

        Size size = ViewCore::sizeForSpace(availableSpace);

        if (bezelStyle != NSBezelStyleRounded) {
            ((NSButton *)nsView()).bezelStyle = bezelStyle;
        }

        return size;
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
