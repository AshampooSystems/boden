#include <bdn/init.h>
#include <bdn/ColumnView.h>


namespace bdn
{

Size ColumnView::calcPreferredSize( const Size& availableSpace ) const
{
	std::list< P<View> > childViews;
	getChildViews(childViews);

    std::list<Rect> childBounds;

	Size usefulSize = calcChildBounds( availableSpace, childViews, childBounds, true );

	return usefulSize;
}

Size ColumnView::calcChildBounds( const Size& availableSpace, const std::list< P<View> >& childViews, std::list<Rect>& childBoundsList, bool forMeasuring) const
{
	Margin myPadding;
        
    Nullable<UiMargin> pad = padding();
    // If padding is null then we use zero padding
    if(!pad.isNull())
        myPadding = uiMarginToDipMargin( pad );

    // clip the max size to the available space.
    Size maxSize = preferredSizeMaximum();
    maxSize.applyMaximum( availableSpace );
    


    bool    widthConstrained = false;
    double  contentAreaWidth = maxSize.width;
    
    if( std::isfinite(contentAreaWidth) )
    {
        // if we have a limited width then we subtract our padding.
        contentAreaWidth = contentAreaWidth - (myPadding.left + myPadding.right);
        if(contentAreaWidth<0)
            contentAreaWidth = 0;

        widthConstrained = true;
    }
    

    double currY = myPadding.top;

    double usefulWidth = myPadding.left + myPadding.top;

	for(const P<View>& pChildView: childViews)
	{
		SizingInfo	childSizingInfo = pChildView->sizingInfo();
        
		Size		childPreferredSize = childSizingInfo.preferredSize;
        
		Margin	    childMargin = pChildView->uiMarginToDipMargin( pChildView->margin() );

        HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

        double childWidthWithMargins = childMargin.left + childPreferredSize.width + childMargin.right;

        if( !forMeasuring && widthConstrained )
        {
            if(childWidthWithMargins > contentAreaWidth)
                childWidthWithMargins = contentAreaWidth;
        }

        bool widthExpanded = false;

        // when we measure the preferred size then we ignore the alignment.
        if(!forMeasuring
            && widthConstrained
            && horzAlign == HorizontalAlignment::expand
            && childWidthWithMargins<contentAreaWidth)
        {
    		childWidthWithMargins = contentAreaWidth;                    
            widthExpanded = true;
        }

        double childWidth = childWidthWithMargins - childMargin.left - childMargin.right;
                
		double childHeight;

		if(childWidth != childPreferredSize.width)
		{
			// the child width is less than the child wanted. So we must get an up-to-date height for the new width.
            // Note that we might also get a new width in the process.
            Size childSize = pChildView->calcPreferredSize( Size(childWidth, Size::componentNone()) );
            
            childHeight = childSize.height;

            // we usually want to use the width we got from preferred size.
            // Exception: if we have previously expanded the width because of an alignment
            // property then we keep the expanded width.
            if(!widthExpanded)
            {
                childWidth = childSize.width;
                childWidthWithMargins = childWidth + childMargin.left + childMargin.right;
            }
		}
		else
			childHeight = childPreferredSize.height;

        double childX = 0;

        // when we measure the preferred size then we ignore the alignment. We want to get
        // the optimal size and the alignment interferes with the child positioning (and sizing
        // when expand alignment is used) because it adapts to the available size. We don't
        // want that during measuring
        if(!forMeasuring && widthConstrained)
        {
            double alignFactor;
		    if(horzAlign == HorizontalAlignment::right)
			    alignFactor = 1;
		    else if(horzAlign == HorizontalAlignment::center)
			    alignFactor = 0.5;
		    else
			    alignFactor = 0;
                    
            childX = (contentAreaWidth-childWidthWithMargins)*alignFactor;
        }
        
        childX += myPadding.left + childMargin.left;
        
        // must round Y up, so that the this child cannot overlap with the previous child
		currY += childMargin.top;

        Rect rawChildBounds( childX, currY, childWidth, childHeight );

        // let the child adjust its bounds to valid values, according to its internal constraints.

        // usually we want to enlarge the child's size to the next valid size, so that its contents
        // will definitely fit. However, if the child's width is already at the maximum then we instead
        // want the resulting size to be rounded down.
        // Exception: when we are measuring then we want the size that the child actually needs for its content,
        // so we should always round up in that case.
        RoundType sizeRoundType  = (widthConstrained && childWidthWithMargins>=contentAreaWidth && !forMeasuring) ? RoundType::down : RoundType::up;

        // we use RoundType::up for positions to ensure that small margins do not disappear. Instead it is better
        // to round them up to 1 pixel, so that separate elements are always actually separate.
        // For example, consider two borderless buttons with flat backgrounds in a single color. The UI designer might arrange
        // them closely next to each other, but with a small space between them. This space works out as 0.4 pixels on this
        // particular display. If we were to use RoundType::down or RoundType::nearest then the space would disappear and
        // it would look to the user as if there was only a single button there. This illustrates that rounding margins
        // up is the better way to go in most cases.
        // Rounding margins up is achieved by rounding positions up. Child view sizes are already adjustes to full pixels,
        // so the only separation between them are margins.
        Rect adjustedChildBounds = pChildView->adjustBounds(
            rawChildBounds,
            RoundType::up,
            sizeRoundType );

        // if the adjustment has modified the child width then we may need to re-calculate the preferred child height.
        if(adjustedChildBounds.width != rawChildBounds.width && adjustedChildBounds.width < childPreferredSize.width)
        {
            double newHeight = pChildView->calcPreferredSize( Size(adjustedChildBounds.width, Size::componentNone()) ).height;
            if(newHeight != adjustedChildBounds.height)
            {
                adjustedChildBounds.height = newHeight;            
                
                // we need to readjust the bounds, size the height changed
                adjustedChildBounds = pChildView->adjustBounds(
                    adjustedChildBounds,
                    RoundType::up,
                    sizeRoundType );
            }
        }

		childBoundsList.push_back( adjustedChildBounds );

        // update childWidthWithMargins
        double childUsefulWidth = adjustedChildBounds.x + adjustedChildBounds.width + childMargin.right + myPadding.right;
        if(childUsefulWidth > usefulWidth)
            usefulWidth = childUsefulWidth;

		currY = adjustedChildBounds.y + adjustedChildBounds.height + childMargin.bottom;
	}

	currY += myPadding.bottom;

    Size prefSize( usefulWidth, currY );

    // apply our minimum size constraint.
    prefSize.applyMinimum( preferredSizeMinimum() );

    // also apply the preferredSizeMaximum. We already tried to arrange our children
    // according to the constrained size, but it may be that they did not fit. In that
    // case prefSize will exceed preferredSizeMaximum and we need to clip it again.
    // Note that we do NOT clip against availableSpace, because we WANT that to be exceeded
    // if the children do not fit. But we never want preferredSizeMaximum to be exceeded.
    prefSize.applyMaximum( preferredSizeMaximum() );

    
    return prefSize;
}

void ColumnView::layout()
{
    Size mySize( size() );

	std::list< P<View> > childViews;
	getChildViews(childViews);

	std::list< Rect >	 childBounds;

	Size usefulSize = calcChildBounds( mySize, childViews, childBounds, false);

	if( usefulSize.height > mySize.height )
	{
		// our content does not fit with the preferred sizes.
		// Todo: Need to implement this. See Issue #1
	}

	auto childBoundsIt = childBounds.begin();
	for(const P<View>& pChildView: childViews)
	{
		Rect childBounds = *childBoundsIt;

        // note that the child bounds are already adjusted. So they will be set without modification
        // here.

		pChildView->adjustAndSetBounds( childBounds );

		++childBoundsIt;
	}
}


}


