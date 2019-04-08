
#import <bdn/mac/SwitchCore.hh>

@implementation BdnSwitchClickManager

- (void)clicked
{
    if (auto core = self.switchCore.lock()) {
        core->updateOn();
        core->_clickCallback.fire();
    }
}

@end

@implementation BdnMacSwitchComposite

- (NSSize)fittingSize
{
    NSSize labelSize = self.label.bounds.size;
    NSSize switchSize = self.bdnSwitch.fittingSize;
    // We only want the switch's size if the label is empty
    CGFloat margin = self.label.stringValue.length > 0 ? 10. : 0.;
    return CGSizeMake(switchSize.width + labelSize.width + margin, fmax(switchSize.height, labelSize.height));
}

- (void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    CGRect switchBounds = _bdnSwitch.bounds;
    CGRect labelBounds = _label.bounds;

    // Center switch vertically in composite
    CGRect switchFrame = CGRectMake(self.bounds.size.width - switchBounds.size.width,
                                    self.bounds.size.height / 2. - switchBounds.size.height / 2.,
                                    switchBounds.size.width, switchBounds.size.height);
    _bdnSwitch.frame = switchFrame;

    // Center label vertically in composite
    CGRect labelFrame = CGRectMake(0, self.bounds.size.height / 2. - labelBounds.size.height / 2.,
                                   labelBounds.size.width, labelBounds.size.height);
    _label.frame = labelFrame;
}

@end

namespace bdn::detail
{
    CORE_REGISTER(Switch, bdn::mac::SwitchCore, Switch)
}

namespace bdn::mac
{
    BdnMacSwitchComposite *SwitchCore::_createSwitchComposite()
    {
        BdnMacSwitchComposite *switchComposite = [[BdnMacSwitchComposite alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

        switchComposite.bdnSwitch = [[BdnMacSwitch alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        [switchComposite addSubview:switchComposite.bdnSwitch];

        switchComposite.label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        switchComposite.label.bezeled = NO;
        switchComposite.label.drawsBackground = NO;
        switchComposite.label.editable = NO;
        switchComposite.label.frame = CGRectMake(0, 0, 100, 20);
        [switchComposite addSubview:switchComposite.label];

        return switchComposite;
    }

    SwitchCore::SwitchCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : bdn::mac::ViewCore(viewCoreFactory, _createSwitchComposite())
    {}

    SwitchCore::~SwitchCore()
    {
        BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
        [composite.bdnSwitch setTarget:nil];
        [composite.bdnSwitch setAction:nil];
    }

    void SwitchCore::init()
    {
        bdn::mac::ViewCore::init();

        _clickManager = [[BdnSwitchClickManager alloc] init];
        _clickManager.switchCore = shared_from_this<SwitchCore>();

        BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
        [composite.bdnSwitch setTarget:_clickManager];
        [composite.bdnSwitch setAction:@selector(clicked)];

        label.onChange() += [=](auto va) {
            BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
            composite.label.stringValue = fk::stringToNSString(va->get());
            NSTextFieldCell *cell = [[NSTextFieldCell alloc] initTextCell:composite.label.stringValue];
            [composite.label setFrameSize:cell.cellSize];
        };

        on.onChange() += [=](auto va) {
            BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
            [composite.bdnSwitch setOn:va->get() animate:NO];
        };
    }

    void SwitchCore::updateOn()
    {
        BdnMacSwitchComposite *composite = (BdnMacSwitchComposite *)nsView();
        on = composite.bdnSwitch.on;
    }
}
