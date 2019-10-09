#import <UIKit/UIKit.h>

@interface UIView (BdnHelper)
- (UIView *)findViewThatIsFirstResponder;
- (UIResponder *)findResponderToHandleKeyboard;
- (double)calculateKeyboardMoveDistanceWithKeyboardSize:(CGSize)kbSize withParentView:(UIView *)parentView;
@end
