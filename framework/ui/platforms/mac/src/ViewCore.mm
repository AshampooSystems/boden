#import <bdn/mac/ViewCore.hh>

#include <iostream>

@interface BdnMacViewCoreEventForwarder_ : NSObject

@property std::weak_ptr<bdn::mac::ViewCore> viewCore;

@end

@implementation BdnMacViewCoreEventForwarder_

- (void)frameDidChange { _viewCore.lock()->frameChanged(); }

@end

namespace bdn::mac
{
    ViewCore::ViewCore(NSView *nsView)
    {
        _nsView = nsView;

        geometry.onChange() += [&view = this->_nsView](auto va) {
            NSRect r = rectToMacRect(va->get(), -1);

            [view setFrameOrigin:r.origin];
            [view setFrameSize:r.size];
        };

        visible.onChange() += [&view = this->_nsView](auto va) { view.hidden = !va->get(); };

        _nsView.postsFrameChangedNotifications = YES;

        /*  _nsView.wantsLayer = YES;
          _nsView.layer.borderColor = [NSColor blueColor].CGColor;
          _nsView.layer.borderWidth = 2;*/
    }

    void ViewCore::init()
    {
        BdnMacViewCoreEventForwarder_ *eventForwarder = [[BdnMacViewCoreEventForwarder_ alloc] init];
        eventForwarder.viewCore = std::dynamic_pointer_cast<ViewCore>(shared_from_this());
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

    Size ViewCore::sizeForSpace(Size availableSpace) const { return macSizeToSize(_nsView.fittingSize); }
}
