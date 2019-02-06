#pragma once

#import <UIKit/UIKit.h>

#include <bdn/TriState.h>

@interface BdnIosCheckbox : UIControl

@property(nonatomic, assign) bdn::TriState checkboxState;
@property(nonatomic, assign) bool isTouching;

- (CGSize)systemLayoutSizeFittingSize:(CGSize)targetSize;
- (void)drawRect:(CGRect)rect;

@end
