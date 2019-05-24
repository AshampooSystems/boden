#import <UIKit/UIKit.h>

@implementation UIView (FindViewThatIsFirstResponder)
- (UIView *)findViewThatIsFirstResponder
{
    if (self.isFirstResponder) {
        return self;
    }

    for (UIView *subView in self.subviews) {
        UIView *firstResponder = [subView findViewThatIsFirstResponder];
        if (firstResponder != nil) {
            return firstResponder;
        }
    }

    return nil;
}

- (UIResponder *)findResponderToHandleKeyboard
{
    UIResponder *responder = self;

    while (responder) {
        if ([responder respondsToSelector:@selector(handleKeyboardWillShow:)]) {
            return responder;
        }

        responder = [responder nextResponder];
    }
    return nil;
}

- (double)calculateKeyboardMoveDistanceWithKeyboardSize:(CGSize)kbSize withParentView:(UIView *)parentView
{
    CGRect frRect = self.frame;
    frRect.origin = CGPointMake(0, 0);
    CGRect relativeRect = [self convertRect:frRect toView:parentView];

    auto remainingHeight = parentView.frame.size.height - kbSize.height;

    if (relativeRect.origin.y + relativeRect.size.height > remainingHeight) {
        auto moveDistance = (relativeRect.origin.y + relativeRect.size.height) - remainingHeight;
        return moveDistance;
    }

    return 0.0;
}

@end
