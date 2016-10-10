#include <bdn/init.h>
#include <bdn/ColumnView.h>


namespace bdn
{

Size ColumnView::calcPreferredSize(double availableWidth, double availableHeight) const
{
	std::list< P<View> > childViews;
	getChildViews(childViews);

    std::list<Rect> childBounds;

    // note that we force left-alignment of the children. That ensures that the "useful size"
    // returned is the minimum size we need. For example, if the availableWidth is much more than
    // what we would ever use then we should return the smaller useful size.

	Size usefulSize = calcChildBoundsForWidth(availableWidth, childViews, childBounds, true );

	return usefulSize;
}

Size ColumnView::calcChildBoundsForWidth(double availableWidth, const std::list< P<View> >& childViews, std::list<Rect>& childBoundsList, bool forceLeftAlignment) const
{
	Margin myPadding;
        
    Nullable<UiMargin> pad = padding();
    // If padding is null then we use zero padding
    if(!pad.isNull())
        myPadding = uiMarginToDipMargin( pad );

    double availableContentAreaWidth;
    
    if(availableWidth==-1)
        availableContentAreaWidth = -1;
    else
    {
        availableContentAreaWidth = availableWidth - (myPadding.left + myPadding.right);
        if(availableContentAreaWidth<0)
            availableContentAreaWidth = 0;
    }

    double currY = myPadding.top;

    double usefulWidth = myPadding.left + myPadding.top;

	for(const P<View>& pChildView: childViews)
	{
		SizingInfo	childSizingInfo = pChildView->sizingInfo();
        
		Size		childPreferredSize = childSizingInfo.preferredSize;
        
		Margin	    childMargin = pChildView->uiMarginToDipMargin( pChildView->margin() );

        HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

        double alignFactor;
		if(horzAlign == HorizontalAlignment::right)
			alignFactor = 1;
		else if(horzAlign == HorizontalAlignment::center)
			alignFactor = 0.5;
		else
			alignFactor = 0;

		double childWidthWithMargins = childMargin.left + childPreferredSize.width + childMargin.right;

        double childX = 0;

        if(availableContentAreaWidth>=0)
        {
            if(childWidthWithMargins > availableContentAreaWidth)
                childWidthWithMargins = availableContentAreaWidth;
        }

        if(!forceLeftAlignment)
        {
            if(availableContentAreaWidth>=0 && horzAlign == HorizontalAlignment::expand)
    			childWidthWithMargins = availableContentAreaWidth;
           
            childX = (availableContentAreaWidth-childWidthWithMargins)*alignFactor;
        }

        childX += myPadding.left + childMargin.left;
        double childWidth = childWidthWithMargins - childMargin.left - childMargin.right;

        childWidth = childWidth;
        
		double childHeight;

		if(childWidth < childPreferredSize.width)
		{
			// the child width is less than the child wanted. So we must get an up-to-date height for the new width.
			childHeight = pChildView->calcPreferredSize(childWidth).height;
		}
		else
			childHeight = childPreferredSize.height;
        
        // must round Y up, so that the this child cannot overlap with the previous child
		currY += childMargin.top;

        Rect rawChildBounds( childX, currY, childWidth, childHeight );

        // let the child adjust its bounds to valid values, according to its internal constraints.

        // usually we want to enlarge the child's size to the next valid size, so that its contents
        // will definitely fit. However, if the child's width is already at the maximum then we instead
        // want the resulting size to be rounded down.
        RoundType sizeRoundType  = (availableContentAreaWidth>=0 && childWidthWithMargins>=availableContentAreaWidth) ? RoundType::down : RoundType::up;

        Rect adjustedChildBounds = pChildView->adjustBounds(
            rawChildBounds,
            RoundType::nearest,XXX see below
            sizeRoundType );

        // if the adjustment has modified the child width then we may need to re-calculate the preferred child height.
        if(adjustedChildBounds.width != rawChildBounds.width && adjustedChildBounds.width < childPreferredSize.width)
        {
            double newHeight = pChildView->calcPreferredSize( adjustedChildBounds.width ).height;;
            if(newHeight != adjustedChildBounds.height)
            {
                adjustedChildBounds.height = newHeight;            
                
                // we need to readjust the bounds, size the height changed
                pChildView->calcPreferredSize( adjustedChildBounds.width ).height;
                adjustedChildBounds = pChildView->adjustBounds(
                    adjustedChildBounds,
                    RoundType::nearest,     XXX nearest is not necessarily correct
                                            case: small margin between controls is wanted. Should it disappear or be 1 pixel?
                                            => should be 1 pixel
                    but what if we have two margins next to each other that are both smaller than 1 pixel? will we get
                    a two pixel combined distance? => no
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
    
    return Size( usefulWidth, currY );
}

void ColumnView::layout()
{
    Size mySize( size() );

	std::list< P<View> > childViews;
	getChildViews(childViews);

	std::list< Rect >	 childBounds;

	Size usefulSize = calcChildBoundsForWidth( mySize.width, childViews, childBounds, false);

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


