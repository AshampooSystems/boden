#ifndef BDN_MAC_MacSwitch_HH_
#define BDN_MAC_MacSwitch_HH_

#import <Cocoa/Cocoa.h>

@class BdnMacSwitch;

@interface BdnSwitchAnimation : NSAnimation

@property (nonatomic, weak) BdnMacSwitch* bdnSwitch;

- (void)setCurrentProgress:(NSAnimationProgress)progress;

@end

@interface BdnMacSwitch : NSControl

@property (nonatomic, assign) bool on;
@property (nonatomic, assign) CGFloat handleOnFactor;
@property (nonatomic, assign) bool isDraggingHandle;
@property (nonatomic, assign) CGFloat startHandleOnFactor;
@property (nonatomic, assign) CGFloat startDragOffset;
@property (nonatomic, strong) BdnSwitchAnimation* animation;
@property (nonatomic, assign) CGFloat handleOnFactorAtAnimationStart;

- (void)setOn:(bool)on animate:(bool)animate;
- (void)animate;

@end

#endif // BDN_MAC_MacSwitch_HH_
