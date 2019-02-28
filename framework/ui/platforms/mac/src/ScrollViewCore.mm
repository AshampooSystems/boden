
#import <bdn/mac/ScrollViewCore.hh>

#import <bdn/mac/util.hh>

#import <Cocoa/Cocoa.h>

/** NSView implementation that is used internally by bdn::mac::ScrollViewCore.

 Sets the flipped property so that the coordinate system has its origin in the
 top left, rather than the bottom left.
 */
@interface BdnMacScrollView_ : NSScrollView

@end

@implementation BdnMacScrollView_

- (BOOL)isFlipped { return YES; }

@end

@interface BdnMacScrollViewContentViewParent_ : NSView

@end

@implementation BdnMacScrollViewContentViewParent_

- (BOOL)isFlipped { return YES; }

@end

@interface BdnMacScrollViewCoreEventForwarder_ : NSObject

@property bdn::mac::ScrollViewCore *scrollViewCore;

@end

@implementation BdnMacScrollViewCoreEventForwarder_

- (void)contentViewBoundsDidChange { _scrollViewCore->_contentViewBoundsDidChange(); }

@end

namespace bdn
{
    namespace mac
    {

        ScrollViewCore::ScrollViewCore(std::shared_ptr<ScrollView> outer)
            : ChildViewCore(outer, _createScrollView(outer))
        {
            _nsScrollView = (NSScrollView *)getNSView();

            // we add a custom view as the document view so that we have better
            // control over the positioning of the content view
            _nsContentViewParent = [[BdnMacScrollViewContentViewParent_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

            _nsScrollView.documentView = _nsContentViewParent;

            _nsScrollView.autohidesScrollers = YES;

            _nsScrollView.contentView.postsBoundsChangedNotifications = YES;

            BdnMacScrollViewCoreEventForwarder_ *eventForwarder = [BdnMacScrollViewCoreEventForwarder_ alloc];
            [eventForwarder setScrollViewCore:this];
            _eventForwarder = eventForwarder;

            [[NSNotificationCenter defaultCenter] addObserver:eventForwarder
                                                     selector:@selector(contentViewBoundsDidChange)
                                                         name:NSViewBoundsDidChangeNotification
                                                       object:_nsScrollView.contentView];

            setHorizontalScrollingEnabled(outer->horizontalScrollingEnabled);
            setVerticalScrollingEnabled(outer->verticalScrollingEnabled);
        }

        ScrollViewCore::~ScrollViewCore()
        {
            [[NSNotificationCenter defaultCenter] removeObserver:_eventForwarder
                                                            name:NSViewBoundsDidChangeNotification
                                                          object:_nsScrollView.contentView];
        }

        NSScrollView *ScrollViewCore::_createScrollView(std::shared_ptr<ScrollView> outer)
        {
            NSScrollView *scrollView = [[BdnMacScrollView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

            return scrollView;
        }

        void ScrollViewCore::addChildNsView(NSView *childView)
        {
            for (id oldViewObject in _nsScrollView.documentView.subviews) {
                NSView *oldView = (NSView *)oldViewObject;
                [oldView removeFromSuperview];
            }

            _nsScrollView.documentView = childView;
        }

        void ScrollViewCore::setHorizontalScrollingEnabled(const bool &enabled)
        {
            _nsScrollView.hasHorizontalScroller = enabled ? YES : NO;
        }

        void ScrollViewCore::setVerticalScrollingEnabled(const bool &enabled)
        {
            _nsScrollView.hasVerticalScroller = enabled ? YES : NO;
        }

        void ScrollViewCore::scrollClientRectToVisible(const Rect &clientRect)
        {
            if (_nsScrollView.contentView != nil) {
                [_nsScrollView.contentView scrollRectToVisible:rectToMacRect(clientRect, -1)];
            }
        }

        void ScrollViewCore::_contentViewBoundsDidChange()
        {
            // when the view scrolls then the bounds of the content view (not
            // the document view) change.
            updateVisibleClientRect();
        }

        void ScrollViewCore::updateVisibleClientRect()
        {
            std::shared_ptr<ScrollView> outerView =
                std::dynamic_pointer_cast<ScrollView>(getOuterViewIfStillAttached());
            if (outerView != nullptr) {
                Rect visibleClientRect = macRectToRect(_nsScrollView.documentVisibleRect, -1);

                outerView->visibleClientRect = visibleClientRect;
            }
        }
    }
}
