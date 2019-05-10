#pragma once

#import <UIKit/UIKit.h>

#include <bdn/ui/TriState.h>

@interface BdnIosCheckbox : UIControl

@property(nonatomic, assign) bdn::ui::TriState checkboxState;
@property(nonatomic, assign) bool isTouching;

- (CGSize)systemLayoutSizeFittingSize:(CGSize)targetSize;
- (void)drawRect:(CGRect)rect;

@end
