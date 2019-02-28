
#import <bdn/mac/SwitchCore.hh>

@implementation BdnSwitchClickManager

- (void)clicked { self.target->clicked(); }

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
