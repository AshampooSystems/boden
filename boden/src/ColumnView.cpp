#include <bdn/init.h>
#include <bdn/ColumnView.h>

#include <bdn/debug.h>


namespace bdn
{

Size ColumnView::calcPreferredSize( const Size& availableSpace ) const
{
    // XXX
    BDN_DEBUGGER_PRINT( String(typeid(*this).name())+".calcPreferredSize("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+"\n"  );

	Size totalSize = calcLayoutImpl( nullptr, availableSpace, true);

    // XXX
            BDN_DEBUGGER_PRINT( "/"+String(typeid(*this).name())+".calcPreferredSize -> "+std::to_string(totalSize.width)+", "+std::to_string(totalSize.height)+")\n" );

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

    Size effectiveAvailableSpace(availableSpace);

    if(forMeasuring)
    {
        // when we are measuring then we restrict the available space to the
        // preferred size maximum.
        effectiveAvailableSpace.applyMaximum( prefSizeMax );
    }

    std::list< P<View> > childViews;
	getChildViews(childViews);

    double contentAvailableWidth = effectiveAvailableSpace.width;
    bool   widthLimited = std::isfinite(contentAvailableWidth);
    if(widthLimited)
    {        
        contentAvailableWidth -= myPadding.left + myPadding.right;
        if(contentAvailableWidth<0)
            contentAvailableWidth = 0;
    }

    double contentAvailableHeight = effectiveAvailableSpace.height;
    bool   heightLimited = std::isfinite(contentAvailableHeight);
    if(heightLimited)
    {
        contentAvailableHeight -= myPadding.top + myPadding.bottom;
        if(contentAvailableHeight<0)
            contentAvailableHeight = 0;
    }

        
	// now we know the width of our content area. We can now proceed to arrange the child views.
	double currY = myPadding.top;

    double endY;
    if(heightLimited)
        endY = currY + contentAvailableHeight;
    else
        endY = std::numeric_limits<double>::infinity();


	for(const P<View>& pChildView: childViews)
	{   
        Margin	    childMargin = pChildView->uiMarginToDipMargin( pChildView->margin() );

        Size        childAvailableSpace( contentAvailableWidth, heightLeft)
        if(widthLimited)
        {
            childAvailableSpace.width -= childMargin.left + childMargin.right;
            if(childAvailableSpace.width<0)
                childAvailableSpace.width = 0;
        }
        if(heightLimited)
        {
            childAvailableSpace.height -= childMargin.top + childMargin.bottom;
            if(childAvailableSpace.height<0)
                childAvailableSpace.height = 0;
        }

		Size		childPreferredSize = pChildView->calcPreferredSize( childAvailableSpace );
                		
		HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

        Size        childSizeWithMargins = childPreferredSize + childMargin;

		// If we are not measuring then the children cannot be bigger than the available space.
        // So, clip in that case.
        if(!forMeasuring)
            childSizeWithMargins.applyMaximum( childAvailableSpace );

		if(horzAlign == HorizontalAlignment::expand && widthLimited && childSizeWithMargins.width < childAvailableSpace.width)
		{
			// expand to the full content area width
			childSizeWithMargins.width = childAvailableSpace.width;
		}

        Size childSize = childSizeWithMargins - childMargin;
        
        double childX = 0;

        double alignFactor;
		if(horzAlign == HorizontalAlignment::right)
			alignFactor = 1;
		else if(horzAlign == HorizontalAlignment::center)
			alignFactor = 0.5;
		else
			alignFactor = 0;

        if(widthLimited)
		    childX = (childAvailableSpace.width - childSizeWithMargins.width) * alignFactor;        
        childX += myPadding.left + childMargin.left;
        
        // must round Y up, so that the this child cannot overlap with the previous child
		currY += childMargin.top;
		
        Rect rawChildBounds( Point(childX, currY), childSize );

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

		// if the adjusted size makes the control larger than the content area size then we need to round the size down instead.
        childSizeWithMargins = ( adjustedChildBounds + childMargin ).getSize();

        if( widthLimited && childSizeWithMargins.width-childAvailableSpace.width > 0.01 )
        {
            // adjusting the child bounds has put the child size slightly over the content area width.
			if(forMeasuring)
			{
				// we can increase the content area width
				contentAvailableWidth = childSizeWithMargins.width;
			}
			else
			{
				// adjust again and this time round the size down.
                sizeRoundType = RoundType::down;

				adjustedChildBounds = pChildView->adjustBounds(
					rawChildBounds,
					RoundType::up,
					sizeRoundType );

                childSizeWithMargins = ( adjustedChildBounds + childMargin ).getSize();
			}
		}

        if( heightLimited && childSizeWithMargins.height>childAvailableSpace.height )
        {
            // the height of the child has exceeded the available space.
            if(forMeasuring)
            {
                // we will increase the total height we want to have. However, since we already
                // used up all available space, we do not need to do that here. All following children
                // will always get 0 available height, whether we adjust immediately or not.
                // So we do nothing here - it makes the loop less complex.
            }
            else
            {
                // we need to clip to the available space.
                childSizeWithMargins.height = childAvailableSpace.height;
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


