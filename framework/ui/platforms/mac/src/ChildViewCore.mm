#import <bdn/mac/ChildViewCore.hh>

#include <iostream>

@interface BdnMacChildViewCoreEventForwarder_ : NSObject

@property bdn::mac::ChildViewCore *childViewCore;

@end

@implementation BdnMacChildViewCoreEventForwarder_

- (void)frameDidChange { _childViewCore->frameChanged(); }

@end

namespace bdn::mac
{
    ChildViewCore::ChildViewCore(std::shared_ptr<View> outer, NSView *nsView)
    {
        _outerView = outer;
        _nsView = nsView;

        geometry.onChange() += [&view = this->_nsView](auto va) {
            NSRect r = rectToMacRect(va->get(), -1);

            [view setFrameOrigin:r.origin];
            [view setFrameSize:r.size];
        };

        visible.onChange() += [&view = this->_nsView](auto va) { view.hidden = !va->get(); };

        _nsView.postsFrameChangedNotifications = YES;

        BdnMacChildViewCoreEventForwarder_ *eventForwarder = [[BdnMacChildViewCoreEventForwarder_ alloc] init];
        eventForwarder.childViewCore = this;
        _eventForwarder = eventForwarder;

        [[NSNotificationCenter defaultCenter] addObserver:eventForwarder
                                                 selector:@selector(frameDidChange)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:_nsView];

        _addToParent(outer->getParentView());

        /*  _nsView.wantsLayer = YES;
          _nsView.layer.borderColor = [NSColor blueColor].CGColor;
          _nsView.layer.borderWidth = 2;*/
    }

    ChildViewCore::~ChildViewCore() { dispose(); }

    void ChildViewCore::moveToParentView(std::shared_ptr<View> newParentView)
    {
        std::shared_ptr<View> outer = outerView();
        if (outer != nullptr) {
            std::shared_ptr<View> parent = outer->getParentView();

            if (newParentView != parent) {
                // Parent has changed. Remove the view from its current
                // super view.
                dispose();
                _addToParent(newParentView);
            }
        }
    }

    void ChildViewCore::dispose() { removeFromNsSuperview(); }

    std::shared_ptr<View> ChildViewCore::outerView() const { return _outerView.lock(); }

    NSView *ChildViewCore::nsView() const { return _nsView; }

    void ChildViewCore::addChildNSView(NSView *childView) { [_nsView addSubview:childView]; }

    void ChildViewCore::removeFromNsSuperview() { [_nsView removeFromSuperview]; }

    void ChildViewCore::frameChanged() { geometry = macRectToRect(_nsView.frame, -1); }

    void ChildViewCore::scheduleLayout() { _nsView.needsLayout = YES; }

    Size ChildViewCore::sizeForSpace(Size availableSpace) const { return macSizeToSize(_nsView.fittingSize); }
}
