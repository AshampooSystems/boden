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

            geometry.onChange() += [&view = this->_nsView](auto &property) {
                NSRect r = rectToMacRect(property.get(), -1);

                [view setFrameOrigin:r.origin];
                [view setFrameSize:r.size];
            };

            visible.onChange() += [&view = this->_nsView](auto &property) { view.hidden = !property.get(); };

            _nsView.postsFrameChangedNotifications = YES;

            if (View::debugViewEnabled()) {
                _nsView.wantsLayer = YES;
                _nsView.layer.borderColor = [NSColor blackColor].CGColor;
                _nsView.layer.borderWidth = 1;

                _nsView.layer.backgroundColor =
                    [NSColor colorWithHue:drand48() saturation:0.75 brightness:0.75 alpha:1.0].CGColor;
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
}
