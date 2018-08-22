#include <bdn/init.h>
#import <bdn/mac/SwitchCore.hh>

@implementation BdnSwitchClickManager

- (void)clicked
{
    self.pTarget->clicked();
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

@end
