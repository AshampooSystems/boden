#import <Cocoa/Cocoa.h>
#import <bdn/mac/ButtonCore.hh>

@interface BdnButtonClickManager : NSObject
@property(nonatomic, assign) std::weak_ptr<bdn::Button> outer;
@end

@implementation BdnButtonClickManager

- (void)clicked
{
    if (auto outer = self.outer.lock()) {
        bdn::ClickEvent clickEvent(outer);
        outer->onClick().notify(clickEvent);
    }
}

@end

namespace bdn::mac
{
    NSButton *ButtonCore::_createNsButton(std::shared_ptr<Button> outerButton)
    {
        NSButton *button = [[NSButton alloc] initWithFrame:rectToMacRect(outerButton->geometry, -1)];

        [button setButtonType:NSButtonTypeMomentaryLight];
        [button setBezelStyle:NSBezelStyleRounded];

        return button;
    }

    ButtonCore::ButtonCore(std::shared_ptr<Button> outer) : ButtonCoreBase(outer, _createNsButton(outer))
    {
        _currBezelStyle = NSBezelStyleRounded;

        _clickManager = [[BdnButtonClickManager alloc] init];
        _clickManager.outer = outer;
        [_nsButton setTarget:_clickManager];
        [_nsButton setAction:@selector(clicked)];

        setLabel(outer->label);

        _heightWithRoundedBezelStyle = macSizeToSize(nsView().fittingSize).height;

        geometry.onChange() += [=](auto) { _updateBezelStyle(); };
    }

    void ButtonCore::setLabel(const String &label) { ButtonCoreBase::setLabel(label); }

    Size ButtonCore::sizeForSpace(Size availableSpace) const
    {
        // the bezel style influences the fitting size. To get
        // consistent values here we have to ensure that we use the same
        // bezel style each time we calculate the size.

        NSBezelStyle bezelStyle = _nsButton.bezelStyle;
        if (bezelStyle != NSBezelStyleRounded)
            _nsButton.bezelStyle = NSBezelStyleRounded;

        Size size = ButtonCoreBase::sizeForSpace(availableSpace);

        if (bezelStyle != NSBezelStyleRounded)
            _nsButton.bezelStyle = bezelStyle;

        return size;
    }

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

        if (geometry->height > _heightWithRoundedBezelStyle * 1.1)
            bezelStyle = NSBezelStyleRegularSquare;
        else
            bezelStyle = NSBezelStyleRounded;

        if (bezelStyle != _currBezelStyle) {
            [_nsButton setBezelStyle:bezelStyle];
        }
    }
}
