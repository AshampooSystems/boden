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

    double pixelsPerDip = getPhysicalPixelsPerDip();
    
	if(availableWidth!=-1)
	{
		std::list<Rect> childBounds;

        // round available size DOWN to full pixels
        availableWidth = RoundDown(pixelsPerDip)(availableWidth);

		double contentEndY = calcChildBoundsForWidth(availableWidth, childViews, childBounds, pixelsPerDip );

        // round height UP to full pixels
        contentEndY = RoundUp(pixelsPerDip)(contentEndY);
	
		return Size(availableWidth, contentEndY);
	}
	else if(availableHeight!=-1)
	{
		// Todo: Need to implement this properly. See issue #1
		return calcPreferredSize(-1, -1);
	}
	else
	{
		Size preferredSize;

        RoundUp upRounder(pixelsPerDip);
	
		for(const P<View>& pChildView: childViews)
		{
			SizingInfo	sizingInfo = pChildView->sizingInfo();
		
			Size		childPreferredSize = sizingInfo.preferredSize;

			childPreferredSize += pChildView->uiMarginToDipMargin( pChildView->margin() );

            // round the child's preferred size UP to pixel boundaries. This is necessary because
            // we added the margin, which might not be a multiple of the pixel size.
            childPreferredSize = upRounder(childPreferredSize);            

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

double ColumnView::calcChildBoundsForWidth(double width, const std::list< P<View> >& childViews, std::list<Rect>& childBounds, double pixelsPerDip) const
{
	Margin myPadding;

    RoundUp     upRounder(pixelsPerDip);
    RoundDown   downRounder(pixelsPerDip);
    
    Nullable<UiMargin> pad = padding();
    // If padding is null then we use zero padding
    if(!pad.isNull())
        myPadding = uiMarginToDipMargin( pad );

	double contentWidth = width - (myPadding.left + myPadding.right);

	double currY = myPadding.top;

	for(const P<View>& pChildView: childViews)
	{
		SizingInfo	childSizingInfo = pChildView->sizingInfo();
		Size		childPreferredSize = childSizingInfo.preferredSize;

		Margin	childMargin = pChildView->uiMarginToDipMargin( pChildView->margin() );

		double maxChildWidth = downRounder(contentWidth - childMargin.left - childMargin.right);

		HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

		double childWidth;
		double childX;
		if(childPreferredSize.width >= maxChildWidth
			|| horzAlign == HorizontalAlignment::expand)
		{
			childWidth = maxChildWidth;
            
			childX = myPadding.left + childMargin.left;
		}
		else
		{
			childWidth = childPreferredSize.width;

			double alignFactor;
			if(horzAlign == HorizontalAlignment::right)
				alignFactor = 1;
			else if(horzAlign == HorizontalAlignment::center)
				alignFactor = 0.5;
			else
				alignFactor = 0;
			
			childX = myPadding.left + childMargin.left + (maxChildWidth-childWidth)*alignFactor;
		}

        // we must round the X coordinate DOWN to the next pixel because otherwise the child might not fit
        // into the container.
        childX = downRounder(childX);

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
		currY += upRounder(childMargin.top);
		
		childBounds.push_back( Rect( childX, currY, childWidth, childHeight ) );

		currY += upRounder(childHeight+ childMargin.bottom);
	}

	currY += myPadding.bottom;

    return upRounder(currY);
}

void ColumnView::layout()
{
    Size mySize( size() );

	std::list< P<View> > childViews;
	getChildViews(childViews);

	std::list< Rect >	 childBounds;
	
	double contentEndY = calcChildBoundsForWidth( mySize.width, childViews, childBounds);

	if( contentEndY > mySize.height )
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


