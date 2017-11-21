#include <bdn/init.h>
#include <bdn/ColumnView.h>

#include <bdn/debug.h>


namespace bdn
{

ColumnView::ColumnView()
{
}


Size ColumnView::calcContainerPreferredSize( const Size& availableSpace ) const
{
    // BDN_DEBUGGER_PRINT( String(typeid(*this).name())+".calcContainerPreferredSize("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+"\n"  );

	Size totalSize = calcLayoutImpl( nullptr, availableSpace, true);

    // BDN_DEBUGGER_PRINT( "/"+String(typeid(*this).name())+".calcContainerPreferredSize -> "+std::to_string(totalSize.width)+", "+std::to_string(totalSize.height)+")\n" );

	return totalSize;
}


P<ViewLayout> ColumnView::calcContainerLayout(const Size& containerSize) const
{
	P<ViewLayout> pLayout = newObj<ViewLayout>();

    calcLayoutImpl(pLayout, containerSize, false);

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

    List< P<View> > childViews;
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
    
    double maxChildWidthWithMargins = 0;

	for(const P<View>& pChildView: childViews)
	{   
        Margin	    childMargin = pChildView->uiMarginToDipMargin( pChildView->margin() );

        // note that we use infinite height. If we exceed the available height then
        // the standard routines from the layout object will be used to decrease the 
        // total height.
        Size        childAvailableSpace( contentAvailableWidth, Size::componentNone() );
        if(widthLimited)
        {
            childAvailableSpace.width -= childMargin.left + childMargin.right;
            if(childAvailableSpace.width<0)
                childAvailableSpace.width = 0;
        }
       

		Size		childPreferredSize = pChildView->calcPreferredSize( childAvailableSpace );

        Size        childSize = childPreferredSize;

        // If we are not measuring then the children cannot be bigger than the available space.
        // So, clip in that case.
        if(!forMeasuring)
            childSize.applyMaximum( childAvailableSpace );
        		
        
		Size        childSizeWithMargins = childSize + childMargin;
		
        // When we are measuring we do not expand the view to the available space if expand alignment is set.
        // Otherwise a container with an expand child would ALWAYS use up all available space, which is not
        // what we want. We want the preferred width for all children to be the container width and THEN
        // expand the children with this alignment to that width.
        // Note that not expanding here means that the preferred child size we got does not actually reflect
        // the final width of the child. That might seem like a problem because we need the accurate child height
        // and width and height can be interdependent. However, we DID pass all available width to the child's
        // calcPreferredSize. And the child said that it cannot use the additional width. So making
        // the view wider should not influence its preferred height.
        // Also, we do not have a function to ask a child "you will have this width, what is your preferred height?"
        // anyway. calcPreferredSize does not force the width, it always gives the child the chance to choose a smaller
        // width. So we couldn't calculate a height for a given width, even if we wanted to.
        
        HorizontalAlignment horzAlign = pChildView->horizontalAlignment();

		if(!forMeasuring
            && horzAlign == HorizontalAlignment::expand
            && widthLimited
            && childSize.width < childAvailableSpace.width)
		{
			// expand to the full content area width
			childSize.width = childAvailableSpace.width;
            childSizeWithMargins = childSize + childMargin;
		}
        
        double childX = 0;

        // we ignore the alignment when we are measuring.
        // We cannot really align correctly, since we do not know the final
        // size of the container yet (only the total available space).
        if(!forMeasuring && widthLimited)
        {
            double alignFactor;
		    if(horzAlign == HorizontalAlignment::right)
    			alignFactor = 1;
		    else if(horzAlign == HorizontalAlignment::center)
			    alignFactor = 0.5;
		    else
    			alignFactor = 0;

            // childAvailableSpace is the space that is available with the child margins subtracted.
            // So we must childSize inside that space, not childSizeWithMargins
            childX = (childAvailableSpace.width - childSize.width) * alignFactor;        
        }
		    
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

        if( widthLimited && childSizeWithMargins.width-contentAvailableWidth > 0.01 )
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

        if(childSizeWithMargins.width > maxChildWidthWithMargins)
            maxChildWidthWithMargins = childSizeWithMargins.width;
	}

	
    Size totalSize( 0, currY + myPadding.bottom );

    if( forMeasuring || !widthLimited)
        totalSize.width = maxChildWidthWithMargins + myPadding.left + myPadding.right;
    else
        totalSize.width = contentAvailableWidth + myPadding.left + myPadding.right;

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


