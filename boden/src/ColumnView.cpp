#include <bdn/init.h>
#include <bdn/ColumnView.h>

#include <bdn/RoundDown.h>
#include <bdn/RoundUp.h>


namespace bdn
{

Size ColumnView::calcPreferredSize(double availableWidth, double availableHeight) const
{
	std::list< P<View> > childViews;
	getChildViews(childViews);

    double pixelSizeDips = getPhysicalPixelSizeInDips();

	if(availableWidth!=-1)
	{
		std::list<Rect> childBounds;

        // round available size DOWN to full pixels
        availableWidth = RoundDown(pixelSizeDips)(availableWidth);

		Size usefulContentSize = calcChildBoundsForWidth(availableWidth, childViews, childBounds, pixelSizeDips );

		return usefulContentSize;
	}
	else if(availableHeight!=-1)
	{
		// Todo: Need to implement this properly. See issue #1
		return calcPreferredSize(-1, -1);
	}
	else
	{
		Size preferredSize;

        RoundUp upRounder(pixelSizeDips);
	
		for(const P<View>& pChildView: childViews)
		{
			SizingInfo	sizingInfo = pChildView->sizingInfo();
		
            // the child's preferred size should already be pixel-aligned. But to make sure we round it again.
            // Note that the rounding is stable (i.e. rounding a previous round result yields the same value).
			Size		childPreferredSize = upRounder( sizingInfo.preferredSize );

			childPreferredSize += upRounder(pChildView->uiMarginToDipMargin( pChildView->margin() ) );
            
			preferredSize.height += childPreferredSize.height;
			preferredSize.width = std::max( preferredSize.width, childPreferredSize.width);
		}

		// add our own padding
		Nullable<UiMargin> pad = padding();
		// If padding is null then we use zero padding (i.e. add nothing)
		if(!pad.isNull())
			preferredSize += upRounder( uiMarginToDipMargin( pad ) );
        
		return preferredSize;
	}
}

Size ColumnView::calcChildBoundsForWidth(double width, const std::list< P<View> >& childViews, std::list<Rect>& childBounds, double pixelSizeDips) const
{
	Margin myPadding;

    RoundUp     upRounder(pixelSizeDips);
    RoundDown   downRounder(pixelSizeDips);
    
    Nullable<UiMargin> pad = padding();
    // If padding is null then we use zero padding
    if(!pad.isNull())
        myPadding = upRounder( uiMarginToDipMargin( pad ) );

	double contentAreaWidth = width - (myPadding.left + myPadding.right);

	double currY = myPadding.top;

    double biggestChildWidthWithMargins = 0;

	for(const P<View>& pChildView: childViews)
	{
		SizingInfo	childSizingInfo = pChildView->sizingInfo();
        
        // the child's preferred size should already be pixel-aligned. But to make sure we round it again.
        // Note that the rounding is stable (i.e. rounding a previous round result yields the same value).
		Size		childPreferredSize = upRounder( childSizingInfo.preferredSize );

		Margin	childMargin = upRounder( pChildView->uiMarginToDipMargin( pChildView->margin() ) );


		HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

		double childWidthWithMargins = childMargin.left + childPreferredSize.width + childMargin.right;

        if(childWidthWithMargins>contentAreaWidth)
            childWidthWithMargins = contentAreaWidth;

        if(childWidthWithMargins > biggestChildWidthWithMargins)
            biggestChildWidthWithMargins = childWidthWithMargins;

        if(horzAlign == HorizontalAlignment::expand)
			childWidthWithMargins = contentAreaWidth;
        
		double alignFactor;
		if(horzAlign == HorizontalAlignment::right)
			alignFactor = 1;
		else if(horzAlign == HorizontalAlignment::center)
			alignFactor = 0.5;
		else
			alignFactor = 0;
			
		double childX = (contentAreaWidth-childWidthWithMargins)*alignFactor;

        childX += myPadding.left + childMargin.left;
        double childWidth = childWidthWithMargins - childMargin.left - childMargin.right;

        childWidth = childWidth;
        
		double childHeight;

		if(childWidth < childPreferredSize.width)
		{
			// the child width is less than the child wanted. So we must get an up-to-date height for the new width.
			childHeight = upRounder( pChildView->calcPreferredSize(childWidth).height );
		}
		else
			childHeight = childPreferredSize.height;
        
        // must round Y up, so that the this child cannot overlap with the previous child
		currY += childMargin.top;
		
		childBounds.push_back( Rect( childX, currY, childWidth, childHeight ) );

		currY += childHeight + childMargin.bottom;
	}

	currY += myPadding.bottom;

    double usefulWidth = myPadding.left + biggestChildWidthWithMargins + myPadding.right;

    return Size( usefulWidth, upRounder(currY) );
}

void ColumnView::layout()
{
    Size mySize( size() );

	std::list< P<View> > childViews;
	getChildViews(childViews);

	std::list< Rect >	 childBounds;

    double pixelSizeDips = getPhysicalPixelSizeInDips();
	
	Size usefulContentSize = calcChildBoundsForWidth( mySize.width, childViews, childBounds, pixelSizeDips);

	if( usefulContentSize.height > mySize.height )
	{
		// our content does not fit with the preferred sizes.
		// Todo: Need to implement this. See Issue #1
	}

	auto childBoundsIt = childBounds.begin();
	for(const P<View>& pChildView: childViews)
	{
		Rect childBounds = *childBoundsIt;
		pChildView->position() = childBounds.getPosition();
        pChildView->size() = childBounds.getSize();

		++childBoundsIt;
	}
}


}


