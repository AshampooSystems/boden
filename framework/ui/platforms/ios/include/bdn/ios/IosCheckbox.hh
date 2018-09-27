#ifndef BDN_IOS_IosCheckbox_HH_
#define BDN_IOS_IosCheckbox_HH_

#import <UIKit/UIKit.h>

#include <bdn/init.h>
#include <bdn/constants.h>

@interface BdnIosCheckbox : UIControl

@property(nonatomic, assign) bdn::TriState checkboxState;
@property(nonatomic, assign) bool isTouching;

- (CGSize)systemLayoutSizeFittingSize:(CGSize)targetSize;
- (void)drawRect:(CGRect)rect;

@end

#endif // BDN_IOS_IosCheckbox_HH_
