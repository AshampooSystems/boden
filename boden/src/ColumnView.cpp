#include <bdn/init.h>
#include <bdn/ColumnView.h>


namespace bdn
{

Size ColumnView::calcPreferredSize( const Size& availableSpace ) const
{
    // XXX
    OutputDebugString( (String(typeid(*this).name())+".calcPreferredSize("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+"\n" ).asWidePtr() );

	Size totalSize = calcLayoutImpl( nullptr, availableSpace, true);

    // XXX
            OutputDebugString( ("/"+String(typeid(*this).name())+".calcPreferredSize()\n" ).asWidePtr() );

	return totalSize;
}


P<ViewLayout> ColumnView::calcLayout(const Size& containerSize) const
{
	P<ViewLayout> pLayout = newObj<ViewLayout>();

	Size resultSize = calcLayoutImpl(pLayout, containerSize, false);

	return pLayout;
}


Size ColumnView::calcLayoutImpl( ViewLayout* pLayout, const Size& availableSpace, bool forMeasuring) const
{
	Margin myPadding;
        
    Nullable<UiMargin> pad = padding();
    // If padding is null then we use zero padding
    if(!pad.isNull())
        myPadding = uiMarginToDipMargin( pad );

    // clip the max size to the available space.
	Size prefSizeMax = preferredSizeMaximum();
     
	    
	std::list< P<View> > childViews;
	getChildViews(childViews);
	
	// First we have to calculate the final width of the container.
	// Otherwise we cannot size our child views properly (since
	// their height might depend on their width).
	double contentAreaWidth = 0;

	if(forMeasuring)
	{
		// when we are measuring then we simply use max(childWidths) as the content area width.

		for(const P<View>& pChildView: childViews)
		{
			SizingInfo	childSizingInfo = pChildView->sizingInfo();
        
			Size		childPreferredSize = childSizingInfo.preferredSize;

			Margin	    childMargin = pChildView->uiMarginToDipMargin( pChildView->margin() );
			
			double childWidthWithMargins = childMargin.left + childPreferredSize.width + childMargin.right;

			if( childWidthWithMargins>contentAreaWidth)
				contentAreaWidth = childWidthWithMargins;
		}

		// the content width can (and should) exceed the available space if our children are bigger.
		// But it cannot exceed the preferred size maximum.
		if( std::isfinite(prefSizeMax.width) )
		{
			double maxContentWidth = prefSizeMax.width - (myPadding.left + myPadding.right);

			if(contentAreaWidth>maxContentWidth)
				contentAreaWidth = maxContentWidth;
		}
	}
	else
	{
		// when we are not measuring the the content area width
		// is already fixed (determined by the size of the container).
		contentAreaWidth = availableSpace.width - (myPadding.left + myPadding.right);        
	}

	if(contentAreaWidth<0)
		contentAreaWidth = 0;

	// now we know the width of our content area. We can now proceed to arrange the child views.
	double currY = myPadding.top;

	for(const P<View>& pChildView: childViews)
	{
		SizingInfo	childSizingInfo = pChildView->sizingInfo();
        
		Size		childPreferredSize = childSizingInfo.preferredSize;
		Margin	    childMargin = pChildView->uiMarginToDipMargin( pChildView->margin() );

		HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

		double childWidthWithMargins = childMargin.left + childPreferredSize.width + childMargin.right;
				
		// clip to the content area width
        if(childWidthWithMargins > contentAreaWidth)
            childWidthWithMargins = contentAreaWidth;

		if(horzAlign == HorizontalAlignment::expand)
		{
			// expand to the full content area width
			childWidthWithMargins = contentAreaWidth;
		}

		double childWidth = childWidthWithMargins - childMargin.left - childMargin.right;                
		double childHeight;

		if( fabs(childWidth-childPreferredSize.width) > 0.01 )
		{
			// the child width is not what the child wanted. So we must get an up-to-date height for the new width.
            // Note that we might also get a new width in the process.
            Size childSize = pChildView->calcPreferredSize( Size(childWidth, Size::componentNone()) );
            
            childHeight = childSize.height;

            // we usually want to use the width we got from preferred size.
            // Exception: if alignment is "expand" then we always keep the expanded width.
			if(horzAlign != HorizontalAlignment::expand)
                childWidth = childSize.width;
		}
		else
		{
			// the child has its preferred width. So it will also get its preferred height.
			childHeight = childPreferredSize.height;
		}

		childWidthWithMargins = childWidth + childMargin.left + childMargin.right;

        double childX = 0;

        double alignFactor;
		if(horzAlign == HorizontalAlignment::right)
			alignFactor = 1;
		else if(horzAlign == HorizontalAlignment::center)
			alignFactor = 0.5;
		else
			alignFactor = 0;
                    
		childX = (contentAreaWidth-childWidthWithMargins)*alignFactor;        
        childX += myPadding.left + childMargin.left;
        
        // must round Y up, so that the this child cannot overlap with the previous child
		currY += childMargin.top;
		
        Rect rawChildBounds( childX, currY, childWidth, childHeight );

        // let the child adjust its bounds to valid values, according to its internal constraints.

        // usually we want to enlarge the child's size to the next valid size, so that its contents
        // will definitely fit. So try that first.
        RoundType sizeRoundType  = RoundType::up;

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

		// if the adjusted width makes the control larger than the content area size then we need to round the size down instead.
		childWidthWithMargins = adjustedChildBounds.width + childMargin.left + childMargin.right;
		if(childWidthWithMargins>contentAreaWidth)
		{
			// adjusting the child bounds has put the child size slightly over the content area width.
			if(forMeasuring)
			{
				// we can increase the content area width
				contentAreaWidth = childWidthWithMargins;
			}
			else
			{
				// if we are significantly bigger than the content area width then we have to adjust again and this time round the
				// size down.
				if( (childWidthWithMargins - contentAreaWidth) > 0.01 )
				{
					sizeRoundType = RoundType::down;

					adjustedChildBounds = pChildView->adjustBounds(
						rawChildBounds,
						RoundType::up,
						sizeRoundType );
				}
			}
		}
		
        // if the adjustment has modified the child width then we may need to re-calculate the preferred child height.
        if( fabs(adjustedChildBounds.width-rawChildBounds.width)>0.01 && adjustedChildBounds.width < childPreferredSize.width)
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
		
		if(pLayout!=nullptr)
		{
			P<ViewLayout::ViewLayoutData> pChildLayoutData = newObj<ViewLayout::ViewLayoutData>();
			pChildLayoutData->setBounds( adjustedChildBounds );		
			pLayout->setViewLayoutData( pChildView, pChildLayoutData );
		}
		
		currY = adjustedChildBounds.y + adjustedChildBounds.height + childMargin.bottom;
	}

	currY += myPadding.bottom;

	Size totalSize( contentAreaWidth + myPadding.left + myPadding.right, currY );

	if(forMeasuring)
	{
		// apply our minimum size constraint.
		totalSize.applyMinimum( preferredSizeMinimum() );
	
		// also apply the preferredSizeMaximum. We already tried to arrange our children
		// according to the constrained size, but it may be that they did not fit. In that
		// case prefSize will exceed preferredSizeMaximum and we need to clip it again.
		// Note that we do NOT clip against availableSpace, because we WANT that to be exceeded
		// if the children do not fit. But we never want preferredSizeMaximum to be exceeded.
		totalSize.applyMaximum( preferredSizeMaximum() );
	}
	    
    return totalSize;
}



}


