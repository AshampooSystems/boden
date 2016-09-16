#include <bdn/init.h>
#include <bdn/ColumnView.h>

#include <cmath>


namespace bdn
{

Size ColumnView::calcPreferredSize(int availableWidth, int availableHeight) const
{
	std::list< P<View> > childViews;
	getChildViews(childViews);

	if(availableWidth!=-1)
	{
		std::list<Rect> childBounds;

		int contentEndY = calcChildBoundsForWidth(availableWidth, childViews, childBounds );
	
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
	
		for(const P<View>& pChildView: childViews)
		{
			SizingInfo	sizingInfo = pChildView->sizingInfo();
		
			Size		childPreferredSize = sizingInfo.preferredSize;

			childPreferredSize += pChildView->uiMarginToPixelMargin( pChildView->margin() );

			preferredSize.height += childPreferredSize.height;
			preferredSize.width = std::max( preferredSize.width, childPreferredSize.width);
		}

		// add our own padding
		Nullable<UiMargin> pad = padding();
		// If padding is null then we use zero padding (i.e. add nothing)
		if(!pad.isNull())
			preferredSize += uiMarginToPixelMargin( pad );

		return preferredSize;
	}
}

int ColumnView::calcChildBoundsForWidth(int width, const std::list< P<View> >& childViews, std::list<Rect>& childBounds) const
{
	Margin myPadding;
    
    Nullable<UiMargin> pad = padding();
    // If padding is null then we use zero padding
    if(!pad.isNull())
        myPadding = uiMarginToPixelMargin( pad );

	int contentWidth = width - (myPadding.left + myPadding.right);

	int currY = myPadding.top;

	for(const P<View>& pChildView: childViews)
	{
		SizingInfo	childSizingInfo = pChildView->sizingInfo();
		Size		childPreferredSize = childSizingInfo.preferredSize;

		Margin	childMargin = pChildView->uiMarginToPixelMargin( pChildView->margin() );

		int maxChildWidth = contentWidth - childMargin.left - childMargin.right;

		HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

		int childWidth;
		int childX;
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
			
			childX = myPadding.left + childMargin.left + (int)std::lround( (maxChildWidth-childWidth)*alignFactor );
		}

		int childHeight;

		if(childWidth < childPreferredSize.width)
		{
			// the child width is less than the child wanted. So we must get an up-to-date height for the new width.
			childHeight = pChildView->calcPreferredSize(childWidth).height;
		}
		else
			childHeight = childPreferredSize.height;

		currY += childMargin.top;
		
		childBounds.push_back( Rect( childX, currY, childWidth, childHeight ) );

		currY += childHeight+ childMargin.bottom;
	}

	currY += myPadding.bottom;

	return currY;
}

void ColumnView::layout()
{
	Rect myBounds = bounds();

	std::list< P<View> > childViews;
	getChildViews(childViews);

	std::list< Rect >	 childBounds;
	
	int contentEndY = calcChildBoundsForWidth( myBounds.width, childViews, childBounds);

	if( contentEndY > myBounds.height )
	{
		// our content does not fit with the preferred sizes.
		// Todo: Need to implement this. See Issue #1
	}

	auto childBoundsIt = childBounds.begin();
	for(const P<View>& pChildView: childViews)
	{
		Rect childBounds = *childBoundsIt;
		pChildView->bounds() = childBounds;

		++childBoundsIt;
	}
}


}


