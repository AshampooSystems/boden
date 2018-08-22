#import <bdn/ios/IosCheckbox.hh>

#define BDN_IOS_CHECKBOX_WIDTH  20
#define BDN_IOS_CHECKBOX_HEIGHT 20

@implementation BdnIosCheckbox

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:CGRectMake(0, 0, BDN_IOS_CHECKBOX_WIDTH, BDN_IOS_CHECKBOX_HEIGHT)];
    if (self) {
        self.opaque = NO;
    }
    return self;
}

- (CGSize)systemLayoutSizeFittingSize:(CGSize)targetSize
{
    return CGSizeMake(BDN_IOS_CHECKBOX_WIDTH, BDN_IOS_CHECKBOX_HEIGHT);
}

- (CGPathRef)roundedRectPathWithRect:(CGRect)outerRect radius:(CGFloat)radius
{
    CGRect innerRect = CGRectInset(outerRect, radius, radius);
 
    CGFloat insideTop = innerRect.origin.y;
    CGFloat outsideTop = outerRect.origin.y;
    CGFloat insideRight = innerRect.origin.x + innerRect.size.width - 1;
    CGFloat outsideRight = outerRect.origin.x + outerRect.size.width - 1;
    CGFloat insideBottom = innerRect.origin.y + innerRect.size.height;
    CGFloat outsideBottom = outerRect.origin.y + outerRect.size.height;
    CGFloat outsideLeft = outerRect.origin.x;
 
    CGMutablePathRef path = CGPathCreateMutable();
    
    CGPathMoveToPoint(path, NULL, innerRect.origin.x, outsideTop);
 
    CGPathAddLineToPoint(path, NULL, insideRight, outsideTop);
    CGPathAddArcToPoint(path, NULL, outsideRight, outsideTop, outsideRight, insideTop, radius);

    CGPathAddLineToPoint(path, NULL, outsideRight, insideBottom);
    CGPathAddArcToPoint(path, NULL,  outsideRight, outsideBottom, insideRight, outsideBottom, radius);
 
    CGPathAddLineToPoint(path, NULL, innerRect.origin.x, outsideBottom);
    CGPathAddArcToPoint(path, NULL,  outsideLeft, outsideBottom, outsideLeft, insideBottom, radius);
    
    CGPathAddLineToPoint(path, NULL, outsideLeft, insideTop);
    CGPathAddArcToPoint(path, NULL,  outsideLeft, outsideTop, innerRect.origin.x, outsideTop, radius);
 
    CGPathCloseSubpath(path);
 
    return path;
}

- (void)drawRect:(CGRect)rect
{
    const CGFloat strokeWidth = 2.5;
    CGRect outerRect = CGRectInset(rect, 1, 1);
    
    UIColor* borderColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.3];
    UIColor* checkmarkColor;
    UIColor* backgroundColor;
    if (self.isTouching) {
        checkmarkColor = [UIColor colorWithRed:1 green:1 blue:1 alpha:1];
        backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.7];
    } else {
        checkmarkColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.8];
        backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
    }

    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // Draw checkbox body background
    CGPathRef roundedRectPath = [self roundedRectPathWithRect:outerRect radius:5];
    CGContextSetFillColorWithColor(context, backgroundColor.CGColor);
    CGContextAddPath(context, roundedRectPath);
    CGContextFillPath(context);

    // Draw checkbox body outline    
    CGContextSetStrokeColorWithColor(context, borderColor.CGColor);
    CGContextAddPath(context, roundedRectPath);
    CGContextStrokePath(context);
    
    if (self.checkboxState == bdn::TriState::on) {
        // Draw checkmark indicating on state
        CGContextSetStrokeColorWithColor(context, checkmarkColor.CGColor);
        CGContextSetLineCap(context, kCGLineCapRound);
        CGContextSetLineWidth(context, strokeWidth);
        CGContextBeginPath(context);
        CGContextMoveToPoint(context, outerRect.origin.x + outerRect.size.width * 0.25, outerRect.origin.y + outerRect.size.height * 0.6);
        CGContextAddLineToPoint(context, outerRect.origin.x + outerRect.size.width * 0.4, outerRect.origin.y + outerRect.size.height * 0.75);
        CGContextAddLineToPoint(context, outerRect.origin.x + outerRect.size.width * 0.75, outerRect.origin.y + outerRect.size.height * 0.25);
        CGContextStrokePath(context);
    } else if (self.checkboxState == bdn::TriState::mixed) {
        // Draw dash indicating mixed state
        CGContextSetStrokeColorWithColor(context, checkmarkColor.CGColor);
        CGContextSetLineCap(context, kCGLineCapRound);
        CGContextSetLineWidth(context, strokeWidth);
        CGContextBeginPath(context);
        CGContextMoveToPoint(context, outerRect.origin.x + outerRect.size.width * 0.25, outerRect.origin.y + outerRect.size.height * 0.5);
        CGContextAddLineToPoint(context, outerRect.origin.x + outerRect.size.width * 0.75, outerRect.origin.y + outerRect.size.height * 0.5);
        CGContextStrokePath(context);
    }
}

- (void)setCheckboxState:(bdn::TriState)checkboxState
{
    if (_checkboxState != checkboxState)
    {
        _checkboxState = checkboxState;
        [self setNeedsDisplay];
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    self.isTouching = YES;
    [self setNeedsDisplay];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    // Change state and send click event only if touch ends inside the control's bounds
    CGPoint touchPoint = [[touches anyObject] locationInView:self];
    if (CGRectContainsPoint(self.bounds, touchPoint))
    {
        switch (self.checkboxState) {
            case bdn::TriState::on:
                self.checkboxState = bdn::TriState::off;
                break;
            case bdn::TriState::mixed:
            case bdn::TriState::off:
                self.checkboxState = bdn::TriState::on;
                break;
        }

        [self sendActionsForControlEvents:UIControlEventTouchUpInside];
        self.isTouching = NO;
    }
}

@end
