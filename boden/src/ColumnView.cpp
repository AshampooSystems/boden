#include <bdn/init.h>
#include <bdn/ColumnView.h>

namespace bdn
{


Size ColumnView::calcPreferredSize() const
{
	Size preferredSize;

	std::list< P<View> > childViews;
	getChildViews(childViews);
	for(const P<View>& pChildView: childViews)
	{
		SizingInfo	sizingInfo = pChildView->sizingInfo();
		
		Size		childPreferredSize = sizingInfo.preferredSize;

		childPreferredSize += pChildView->uiMarginToPixelMargin( pChildView->margin() );

		preferredSize.height += childPreferredSize.height;
		preferredSize.width = std::max( preferredSize.width, childPreferredSize.width);
	}

	// add our own padding
	preferredSize += uiMarginToPixelMargin( padding() );

	return preferredSize;
}

int ColumnView::calcPreferredHeightForWidth(int width) const
{
	std::list< P<View> > childViews;
	getChildViews(childViews);

	std::list<Rect> childBounds;

	int contentEndY = calcChildBoundsForWidth(width, childViews, childBounds );

	return contentEndY;
}


int ColumnView::calcPreferredWidthForHeight(int height) const
{
	// Todo: Need to implement this properly. See issue #1
	return calcPreferredSize().width;
}



int ColumnView::calcChildBoundsForWidth(int width, const std::list< P<View> >& childViews, std::list<Rect>& childBounds) const
{
	Margin myPadding = uiMarginToPixelMargin( padding() );

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
			
			childX = myPadding.left + childMargin.left + std::lround( (maxChildWidth-childWidth)*alignFactor );
		}

		int childHeight;

		if(childWidth < childPreferredSize.width)
		{
			// the child width is less than the child wanted. So we must get an up-to-date height for the new width.
			childHeight = pChildView->calcPreferredHeightForWidth(childWidth);
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


