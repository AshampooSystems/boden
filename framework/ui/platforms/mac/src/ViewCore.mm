#import <bdn/mac/ViewCore.hh>

#include <iostream>

@interface BdnMacViewCoreEventForwarder_ : NSObject

@property std::weak_ptr<bdn::ui::mac::ViewCore> viewCore;

@end

@implementation BdnMacViewCoreEventForwarder_

- (void)frameDidChange { self.viewCore.lock()->frameChanged(); }

@end

namespace bdn::ui::mac
{
    ViewCore::ViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, NSView *nsView)
        : bdn::ui::View::Core(viewCoreFactory), _nsView(nsView)
    {
        if (_nsView != nullptr) {

            geometry.onChange() += [this](auto &property) { setFrame(property.get()); };

            visible.onChange() += [&view = this->_nsView](auto &property) { view.hidden = !property.get(); };

            backgroundColor.onChange() += [this](auto &property) {
                auto color = property.get();

                if (color) {
                    _nsView.wantsLayer = YES;
                    _nsView.layer.backgroundColor =
                        [NSColor colorWithRed:color->red() green:color->green() blue:color->blue() alpha:color->alpha()]
                            .CGColor;
                } else {
                    _nsView.layer.backgroundColor = [NSColor clearColor].CGColor;
                    _nsView.wantsLayer = NO;
                }
            };

            _nsView.postsFrameChangedNotifications = YES;

            if (View::debugViewEnabled()) {
                _nsView.wantsLayer = YES;
                _nsView.layer.borderColor = [NSColor blackColor].CGColor;
                _nsView.layer.borderWidth = 1;

                _nsView.layer.backgroundColor =
                    [NSColor colorWithHue:drand48() saturation:0.75 brightness:0.75 alpha:1.0].CGColor;
            }

            if (View::debugViewBaselineEnabled()) {
                _baselineIndicator = [[NSView alloc] init];
                _baselineIndicator.wantsLayer = YES;
                _baselineIndicator.hidden = YES;
                _baselineIndicator.layer.backgroundColor =
                    [NSColor colorWithRed:1.0 green:0.0 blue:0.0 alpha:0.75].CGColor;

                [_nsView addSubview:_baselineIndicator];
            }
        }
    }

    void ViewCore::init()
    {
        BdnMacViewCoreEventForwarder_ *eventForwarder = [[BdnMacViewCoreEventForwarder_ alloc] init];
        eventForwarder.viewCore = shared_from_this<ViewCore>();
        _eventForwarder = eventForwarder;

        [[NSNotificationCenter defaultCenter] addObserver:eventForwarder
                                                 selector:@selector(frameDidChange)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:_nsView];
    }

    NSView *ViewCore::nsView() const { return _nsView; }

    void ViewCore::addChildNSView(NSView *childView) { [_nsView addSubview:childView]; }

    void ViewCore::removeFromNsSuperview() { [_nsView removeFromSuperview]; }

    void ViewCore::frameChanged() { geometry = macRectToRect(_nsView.frame, -1); }

    void ViewCore::scheduleLayout() { _nsView.needsLayout = YES; }

    Size ViewCore::sizeForSpace(Size) const { return macSizeToSize(_nsView.fittingSize); }

    float ViewCore::baseline(Size forSize) const
    {
        auto cb = calculateBaseline(forSize, false);

        if (View::debugViewBaselineEnabled()) {
            _baselineIndicator.hidden = NO;
            _baselineIndicator.frame = CGRectMake(0, calculateBaseline(forSize, true), forSize.width, 1);
        }

        return cb;
    }

    float ViewCore::calculateBaseline(Size forSize, bool forIndicator) const
    {
        return static_cast<float>(forSize.height);
    }

    float ViewCore::pointScaleFactor() const
    {
        if (_nsView && _nsView.window) {
            return static_cast<float>(_nsView.window.backingScaleFactor);
        }
        return 1.0f;
    }

    void ViewCore::setFrame(Rect r)
    {
        NSRect frame = rectToMacRect(r, -1);

        [_nsView setFrameOrigin:frame.origin];
        [_nsView setFrameSize:frame.size];
    }
}
