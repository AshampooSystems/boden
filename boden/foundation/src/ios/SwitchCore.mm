#include <bdn/init.h>

#include <bdn/ios/SwitchCore.hh>

@implementation BdnIosSwitchComposite

// Use systemLayoutSizeFittingSize instead of intrinsicContentSize
- (CGSize)systemLayoutSizeFittingSize:(CGSize)targetSize
{
    CGFloat minMargin = 10.;
    CGSize switchSize = [self.uiSwitch systemLayoutSizeFittingSize:targetSize];
    CGSize labelSize = [self.uiLabel systemLayoutSizeFittingSize:targetSize];
    return CGSizeMake(switchSize.width + labelSize.width + minMargin,
                      fmax(switchSize.height, labelSize.height));
}

@end
