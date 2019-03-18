#import <bdn/mac/MacSwitch.hh>

#define BDN_MAC_SWITCH_WIDTH 42
#define BDN_MAC_SWITCH_HEIGHT 20
#define BDN_MAC_SWITCH_HANDLE_MARGIN 1
#define BDN_MAC_SWITCH_HANDLE_DROP_SHADOW_OFFSET 1

@implementation BdnMacSwitch

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:NSMakeRect(0, 0, BDN_MAC_SWITCH_WIDTH, BDN_MAC_SWITCH_HEIGHT)];
    return self;
}

- (void)dealloc { self.animation = nil; }

- (void)setOn:(bool)on
{
    _on = on;
    [self animate];
}

- (void)setOn:(bool)on animate:(bool)animate
{
    if (_on != on) {
        _on = on;
        if (animate) {
            [self animate];
        } else {
            self.handleOnFactor = _on ? 1.0 : 0.0;
            self.handleOnFactorAtAnimationStart = self.handleOnFactor;
        }
    }
}

- (void)setHandleOnFactor:(CGFloat)handleOnFactor
{
    _handleOnFactor = handleOnFactor;
    [self setNeedsDisplay:YES];
}

- (NSSize)fittingSize { return NSMakeSize(BDN_MAC_SWITCH_WIDTH, BDN_MAC_SWITCH_HEIGHT); }

- (void)drawRoundedRectPathWithRect:(CGRect)outerRect
                             radius:(CGFloat)radius
                              color:(CGColorRef)color
                            context:(CGContextRef)context
{
    CGRect innerRect = CGRectMake(outerRect.origin.x + radius, outerRect.origin.y + radius,
                                  outerRect.size.width - radius * 2, outerRect.size.height - radius * 2);

    CGFloat insideTop = innerRect.origin.y;
    CGFloat outsideTop = outerRect.origin.y;
    CGFloat insideRight = innerRect.origin.x + innerRect.size.width - 1;
    CGFloat outsideRight = outerRect.origin.x + outerRect.size.width - 1;
    CGFloat insideBottom = innerRect.origin.y + innerRect.size.height;
    CGFloat outsideBottom = outerRect.origin.y + outerRect.size.height;
    CGFloat outsideLeft = outerRect.origin.x;

    CGMutablePathRef path = CGPathCreateMutable();

    CGPathMoveToPoint(path, nullptr, innerRect.origin.x, outsideTop);

    CGPathAddLineToPoint(path, nullptr, insideRight, outsideTop);
    CGPathAddArcToPoint(path, nullptr, outsideRight, outsideTop, outsideRight, insideTop, radius);

    CGPathAddLineToPoint(path, nullptr, outsideRight, insideBottom);
    CGPathAddArcToPoint(path, nullptr, outsideRight, outsideBottom, insideRight, outsideBottom, radius);

    CGPathAddLineToPoint(path, nullptr, innerRect.origin.x, outsideBottom);
    CGPathAddArcToPoint(path, nullptr, outsideLeft, outsideBottom, outsideLeft, insideBottom, radius);

    CGPathAddLineToPoint(path, nullptr, outsideLeft, insideTop);
    CGPathAddArcToPoint(path, nullptr, outsideLeft, outsideTop, innerRect.origin.x, outsideTop, radius);

    CGPathCloseSubpath(path);

    CGContextSetFillColorWithColor(context, color);
    CGContextAddPath(context, path);
    CGContextFillPath(context);

    CGPathRelease(path);
}

- (CGFloat)handleRadius { return self.bounds.size.height - BDN_MAC_SWITCH_HANDLE_MARGIN * 2; }

- (CGFloat)handlePosition
{
    CGFloat handlePositionOn = self.bounds.size.width - BDN_MAC_SWITCH_HANDLE_MARGIN * 2 - self.handleRadius;
    CGFloat handlePositionOff = BDN_MAC_SWITCH_HANDLE_MARGIN;
    return handlePositionOff + self.handleOnFactor * (handlePositionOn - handlePositionOff);
}

- (CGRect)handleRect { return CGRectMake(self.handlePosition, 1, self.handleRadius, self.handleRadius); }

- (void)drawRect:(NSRect)rect
{
    NSColor *backgroundColorOff = [NSColor colorWithRed:0 green:0 blue:0 alpha:0.5];
    NSColor *backgroundColorOn = [NSColor colorWithRed:0 green:0.8 blue:0 alpha:1];
    NSColor *handleColor = [NSColor colorWithRed:1 green:1 blue:1 alpha:1];
    NSColor *handleDropShadowColorOff = [NSColor colorWithRed:0 green:0 blue:0 alpha:0.25];
    NSColor *handleDropShadowColorOn = [NSColor colorWithRed:0 green:0 blue:0 alpha:0.1];

    CGRect outerRect = self.bounds;

    CGFloat handlePosition = self.handlePosition;
    CGFloat handleDropShadowPosition = handlePosition - BDN_MAC_SWITCH_HANDLE_DROP_SHADOW_OFFSET +
                                       self.handleOnFactor * BDN_MAC_SWITCH_HANDLE_DROP_SHADOW_OFFSET * 2;
    NSColor *backgroundColor =
        [backgroundColorOff blendedColorWithFraction:self.handleOnFactor ofColor:backgroundColorOn];
    NSColor *handleDropShadowColor =
        [handleDropShadowColorOff blendedColorWithFraction:self.handleOnFactor ofColor:handleDropShadowColorOn];

    CGRect handleRect = self.handleRect;
    CGRect handleDropShadowRect =
        CGRectMake(handleDropShadowPosition, 1, handleRect.size.width, handleRect.size.height);

    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];

    // Draw switch body backround
    [self drawRoundedRectPathWithRect:outerRect
                               radius:self.bounds.size.height / 2
                                color:backgroundColor.CGColor
                              context:context];

    // Draw handle drop shadow
    CGContextSetFillColorWithColor(context, handleDropShadowColor.CGColor);
    CGContextFillEllipseInRect(context, handleDropShadowRect);

    // Draw handle body
    CGContextSetFillColorWithColor(context, handleColor.CGColor);
    CGContextFillEllipseInRect(context, handleRect);
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint locationInView = [self convertPoint:event.locationInWindow fromView:nil];
    self.startDragOffset = locationInView.x;
    self.startHandleOnFactor = self.handleOnFactor;
    self.isDraggingHandle = YES;
}

- (void)mouseDragged:(NSEvent *)event
{
    CGFloat handleRadius = self.bounds.size.height - BDN_MAC_SWITCH_HANDLE_MARGIN * 2;
    NSPoint locationInView = [self convertPoint:event.locationInWindow fromView:nil];
    CGFloat dragOffset = locationInView.x - self.startDragOffset;
    CGFloat dragWidth = self.bounds.size.width - BDN_MAC_SWITCH_HANDLE_MARGIN * 2 - handleRadius;
    CGFloat handleOnFactor = self.startHandleOnFactor + dragOffset / dragWidth;
    handleOnFactor = fmin(handleOnFactor, 1.0);
    handleOnFactor = fmax(handleOnFactor, 0.0);
    self.handleOnFactor = handleOnFactor;
}

- (void)mouseUp:(NSEvent *)event
{
    NSPoint locationInView = [self convertPoint:event.locationInWindow fromView:nil];
    if (locationInView.x < self.startDragOffset + 5 && locationInView.x > self.startDragOffset - 5) {
        self.on = !self.on;
    } else if (locationInView.x < self.startDragOffset) {
        self.on = false;
    } else {
        self.on = true;
    }

    [self sendAction:self.action to:self.target];

    [self animate];
    self.isDraggingHandle = NO;
}

- (void)animate
{
    if ((self.animation != nullptr) && (self.animation.isAnimating != 0)) {
        return;
    }

    if ((self.on && self.handleOnFactor == 1.0) || (!self.on && self.handleOnFactor == 0.0)) {
        return;
    }

    self.animation = [[BdnSwitchAnimation alloc] initWithDuration:0.1 animationCurve:NSAnimationEaseOut];
    self.animation.bdnSwitch = self;
    self.animation.animationBlockingMode = NSAnimationNonblocking;
    self.handleOnFactorAtAnimationStart = self.handleOnFactor;
    [self.animation startAnimation];
}

@end

@implementation BdnSwitchAnimation

- (void)setCurrentProgress:(NSAnimationProgress)progress
{
    CGFloat length = self.bdnSwitch.on ? 1.0 - self.bdnSwitch.handleOnFactorAtAnimationStart
                                       : self.bdnSwitch.handleOnFactorAtAnimationStart;
    CGFloat start = self.bdnSwitch.on ? self.bdnSwitch.handleOnFactorAtAnimationStart : 0.0;
    self.bdnSwitch.handleOnFactor = start + (!self.bdnSwitch.on ? 1.0 - (CGFloat)progress : (CGFloat)progress) * length;
}

@end
