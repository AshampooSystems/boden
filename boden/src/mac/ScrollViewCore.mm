#include <bdn/init.h>
#import <bdn/mac/ScrollViewCore.hh>

#import <Cocoa/Cocoa.h>

/** NSView implementation that is used internally by bdn::mac::ScrollViewCore.
 
 Sets the flipped property so that the coordinate system has its origin in the top left,
 rather than the bottom left.
 */
@interface BdnMacScrollView_ : NSScrollView

@end


@implementation BdnMacScrollView_

- (BOOL) isFlipped
{
    return YES;
}

@end


namespace bdn
{
namespace mac
{


NSScrollView* ScrollViewCore::_createScrollView(ScrollView* pOuter)
{
    return [[BdnMacScrollView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
}

void ScrollViewCore::addChildNsView( NSView* childView )
{
    _nsScrollView.documentView = childView;
}

void ScrollViewCore::setHorizontalScrollingEnabled(const bool& enabled)
{
    
}

void ScrollViewCore::setVerticalScrollingEnabled(const bool& enabled)
{
    
}

    
Size ScrollViewCore::calcPreferredSize( const Size& availableSpace ) const
{
    return ChildViewCore::calcPreferredSize(availableSpace);
}

void ScrollViewCore::layout()
{
    // nothing to do - macOS takes care of arranging the document view.
}


}
}
