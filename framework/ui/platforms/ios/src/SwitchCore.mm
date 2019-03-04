#include <bdn/ios/SwitchCore.hh>

@implementation BdnIosSwitchClickManager

- (void)clicked
{
    if (auto outer = self.outer.lock()) {
        bdn::ClickEvent clickEvent(outer);
        outer->onClick().notify(clickEvent);
    }
}

@end

@implementation BdnIosSwitchComposite

// Use systemLayoutSizeFittingSize instead of intrinsicContentSize
- (CGSize)systemLayoutSizeFittingSize:(CGSize)targetSize
{
    CGFloat minMargin = 10.;
    CGSize switchSize = [self.uiSwitch systemLayoutSizeFittingSize:targetSize];
    CGSize labelSize = [self.uiLabel systemLayoutSizeFittingSize:targetSize];
    return CGSizeMake(switchSize.width + labelSize.width + minMargin, fmax(switchSize.height, labelSize.height));
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}
@end

namespace bdn::ios
{
    BdnIosSwitchComposite *SwitchCore::createSwitchComposite()
    {
        BdnIosSwitchComposite *switchComposite = [[BdnIosSwitchComposite alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];

        switchComposite.uiSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        [switchComposite addSubview:switchComposite.uiSwitch];

        switchComposite.uiLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        [switchComposite addSubview:switchComposite.uiLabel];

        return switchComposite;
    }

    SwitchCore::SwitchCore(std::shared_ptr<Switch> outer) : ViewCore(outer, createSwitchComposite())
    {
        _composite = (BdnIosSwitchComposite *)uiView();

        _clickManager = [[BdnIosSwitchClickManager alloc] init];
        _clickManager.outer = outer; // reference outer instead of core

        [_composite.uiSwitch addTarget:_clickManager
                                action:@selector(clicked)
                      forControlEvents:UIControlEventTouchUpInside];

        // Set initial state
        setLabel(outer->label);
        setOn(outer->on);

        geometry.onChange() += [=](auto va) {
            BdnIosSwitchComposite *switchComposite = (BdnIosSwitchComposite *)_composite;

            CGRect compositeBounds = switchComposite.bounds;
            CGRect switchBounds = switchComposite.uiSwitch.bounds;
            CGRect labelBounds = switchComposite.uiLabel.bounds;

            // Center switch vertically in composite
            CGRect switchFrame = CGRectMake(compositeBounds.size.width - switchBounds.size.width,
                                            compositeBounds.size.height / 2. - switchBounds.size.height / 2.,
                                            switchBounds.size.width, switchBounds.size.height);
            switchComposite.uiSwitch.frame = switchFrame;

            // Center label vertically in composite
            CGRect labelFrame = CGRectMake(0, compositeBounds.size.height / 2. - labelBounds.size.height / 2.,
                                           labelBounds.size.width, labelBounds.size.height);
            switchComposite.uiLabel.frame = labelFrame;
        };
    }

    SwitchCore::~SwitchCore()
    {
        BdnIosSwitchComposite *switchComposite = (BdnIosSwitchComposite *)_composite;
        [switchComposite.uiSwitch removeTarget:_clickManager action:nil forControlEvents:UIControlEventTouchUpInside];
    }

    void SwitchCore::setOn(const bool &on) { ((BdnIosSwitchComposite *)_composite).uiSwitch.on = on; }

    void SwitchCore::setLabel(const String &label)
    {
        _composite.uiLabel.text = stringToNSString(label);
        [_composite.uiLabel sizeToFit];
    }
}
