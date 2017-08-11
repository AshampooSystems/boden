#include <bdn/init.h>
#import <bdn/ios/ScrollViewCore.hh>

#include <bdn/ScrollViewLayoutHelper.h>


namespace bdn
{
namespace ios
{

ScrollViewCore::ScrollViewCore(	ScrollView* pOuter)
    : ViewCore(pOuter, _createScrollView(pOuter) )
{
    _uiScrollView = (UIScrollView*)getUIView();
    

    // we add a custom view as the document view so that we have better control over the positioning
    // of the content view
    _uiContentViewParent = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
    
    [_uiScrollView addSubview: _uiContentViewParent];
    
    setHorizontalScrollingEnabled( pOuter->horizontalScrollingEnabled() );
    setVerticalScrollingEnabled( pOuter->verticalScrollingEnabled() );
    
    
    setPadding( pOuter->padding() );
}

UIScrollView* ScrollViewCore::_createScrollView(ScrollView* pOuter)
{
    return [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
}



void ScrollViewCore::addChildUIView( UIView* childView )
{    
    for(id subView in _uiContentViewParent.subviews)
        [((UIView*)subView) removeFromSuperview];
    
    [_uiContentViewParent addSubview:childView];    
}

void ScrollViewCore::setPadding(const Nullable<UiMargin>& padding)
{
    // nothing to do
}


void ScrollViewCore::setHorizontalScrollingEnabled(const bool& enabled)
{
    // we can only enable scrolling for both directions at once.
    // However, that is not a problem: we will clip the content to make it fit in
    // directions in which OUR scrollingEnabled is false.
    
    _horzScrollEnabled = enabled;
    
    _uiScrollView.scrollEnabled = _horzScrollEnabled || _vertScrollEnabled;
}

void ScrollViewCore::setVerticalScrollingEnabled(const bool& enabled)
{
    _vertScrollEnabled = enabled;
    
    _uiScrollView.scrollEnabled = _horzScrollEnabled || _vertScrollEnabled;
}
    
Size ScrollViewCore::calcPreferredSize( const Size& availableSpace ) const
{
    Size preferredSize;

    P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
    {
        ScrollViewLayoutHelper helper(0,0);
        
        preferredSize = helper.calcPreferredSize( pOuterView, availableSpace);
    }
    
    return preferredSize;
}

void ScrollViewCore::layout()
{
    P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
    {
        ScrollViewLayoutHelper helper(0,0);
        
        Size viewPortSizeWithoutScrollbars = pOuterView->size();
        
        helper.calcLayout(pOuterView, viewPortSizeWithoutScrollbars);
        
        CGSize iosScrolledAreaSize{0,0};
        
        P<View> pContentView = pOuterView->getContentView();
        if(pContentView!=nullptr)
        {
            Rect contentBounds = helper.getContentViewBounds();
            
            pContentView->adjustAndSetBounds( contentBounds );
            
            // we must also resize our content view parent accordingly.
            Size scrolledAreaSize = helper.getScrolledAreaSize();
            
            iosScrolledAreaSize = sizeToIosSize(scrolledAreaSize);
        }
        
        _uiContentViewParent.frame = CGRectMake(
                                                _uiContentViewParent.frame.origin.x,
                                                _uiContentViewParent.frame.origin.y,
                                                iosScrolledAreaSize.width,
                                                iosScrolledAreaSize.height );
        
       
        // we must set the contentSize property of the scroll view. IOS uses
        // it to determine whether to scroll and by how much
        CGSize iosScrollContentSize = iosScrolledAreaSize;
        
        // one important aspect here: if scrolling is disabled in a direction
        // then we should set the scrollContentSize to some value <= the viewport size
        // to ensure that no scrolling actually happens. That is important, since
        // UIScrollView can only disable scrolling in both directions at once,
        // not individually. So if we want one direction enabled and the other
        // disabled then we must ensure that the content size in the disabled
        // direction does not exceed the viewport size.
        CGSize iosViewPortSize = _uiScrollView.frame.size;
        
        if( !pOuterView->horizontalScrollingEnabled().get() && iosScrollContentSize.width > iosViewPortSize.width )
            iosScrollContentSize.width = iosViewPortSize.width;

        if( !pOuterView->verticalScrollingEnabled().get() && iosScrollContentSize.height > iosViewPortSize.height )
            iosScrollContentSize.height = iosViewPortSize.height;
        
        _uiScrollView.contentSize = iosScrollContentSize;        
        
    }
}


}
}
