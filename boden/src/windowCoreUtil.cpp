#include <bdn/init.h>
#include <bdn/windowCoreUtil.h>

#include <bdn/mainThread.h>


namespace bdn
{

Size defaultWindowCalcPreferredSizeImpl(Window* pWindow, const Size& availableSpace, const Margin& border, const Size& minWindowSize )
{
    Margin contentMargin;
	P<const View>	pContentView = pWindow->getContentView();
	if(pContentView!=nullptr)
		contentMargin = pContentView->uiMarginToDipMargin( pContentView->margin() );

    Margin padding;    
    // default padding is zero
    Nullable<UiMargin> pad = pWindow->padding();
    if(!pad.isNull())            
        padding = pWindow->uiMarginToDipMargin( pad );


    // combine maxSize with availableSpace
    Size maxSize = pWindow->preferredSizeMaximum();
    maxSize.applyMaximum(availableSpace);

	Size availableContentSpace( maxSize );

    // subtract the nonclient border, padding and the content view margin
    if(std::isfinite( availableContentSpace.width ) )
    {        
        availableContentSpace.width -= border.left + border.right + padding.left + padding.right + contentMargin.left + contentMargin.right;
        if(availableContentSpace.width < 0)
            availableContentSpace.width = 0;
    }
    if(std::isfinite( availableContentSpace.height ) )
    {    
        availableContentSpace.height -= border.top + border.bottom + padding.top + padding.bottom + contentMargin.top + contentMargin.bottom;
        if(availableContentSpace.height < 0)
            availableContentSpace.height = 0;
    }

		
	Size contentSize;
	if(pContentView!=nullptr)
		contentSize = pContentView->calcPreferredSize( availableContentSpace );

	Size preferredSize = contentSize + contentMargin + padding + border;
    
    // apply minimum size constraint (the maximum constraint has already been applied above)
    preferredSize.applyMinimum( pWindow->preferredSizeMinimum() );

    // also apply the platform's minimm window size
    preferredSize.applyMinimum( minWindowSize );


    // also apply the preferredSizeMaximum. We already applied it at the start to
    // take the constraint into account from the beginning, but it may be that prefSize
    // is bigger than the max here because the content window does not fit.
    // So we clip the result against the max here, because we never want it to be exceeded.
    // Note that we do NOT clip against availableSpace, because we WANT that to be exceeded
    // if the children do not fit.
    preferredSize.applyMaximum( pWindow->preferredSizeMaximum() );

	return preferredSize;
}




void defaultWindowLayoutImpl(Window* pWindow, const Rect& contentArea)
{
	P<View>			pContentView = pWindow->getContentView();

	if(pContentView!=nullptr)
	{    
	    // just set our content window to content area (but taking margins and padding into account).
        Rect contentBounds( contentArea );
	    
        Margin padding;    
        // default padding is zero
        Nullable<UiMargin> pad = pWindow->padding();
        if(!pad.isNull())            
            padding = pWindow->uiMarginToDipMargin( pad );

        // subtract our padding
	    contentBounds -= padding;

	    // subtract the content view's margins
	    contentBounds -= pContentView->uiMarginToDipMargin( pContentView->margin() );
    
        pContentView->adjustAndSetBounds( contentBounds );
    
	    // note that we do not need to call layout on the content view.
	    // If it needs to update its layout then the bounds change should have caused
	    // it to schedule an update.
    }
}

void defaultWindowAutoSizeImpl(Window* pWindow, const Size& screenWorkAreaSize)
{
	Rect newBounds;
    
	Size preferredSize = pWindow->calcPreferredSize();
        	
	double width = preferredSize.width;
	double height = preferredSize.height;
    
	if(width > screenWorkAreaSize.width)
	{
		// we do not fit on the screen at our preferred width.
		// So we reduce the width to the maximum allowed width.
		width = screenWorkAreaSize.width;

		// and then adapt the height accordingly (height might increase if we reduce the width).
		height = pWindow->calcPreferredSize( Size(width, Size::componentNone()) ).height;	

		// if the height we calculated is bigger than the max height then we simply
		// cannot achieve our preferred size. We will have to make do with the max available size.
		if(height > screenWorkAreaSize.height)
			height = screenWorkAreaSize.height;
	}

	if(height > screenWorkAreaSize.height)
	{
		// height does not fit. Reduce it so that it fits.
		height = screenWorkAreaSize.height;

		// and then adapt the width accordingly.
		width = pWindow->calcPreferredSize( Size( Size::componentNone(), height) ).width;	

		// if the width we calculated is bigger than the max width then we simply
		// cannot achieve our preferred size. We will have to make do with the max available size.
		if(width > screenWorkAreaSize.width)
			width = screenWorkAreaSize.width;
	}

    // we want to round the size up always. If the window does not exceed the screen size
    // then we want all our content to fit guaranteed. And if the window size previously exceeded the screen size
    // then it has been clipped to the screen size. And we assume that the screen size is a valid size
    // for the display and rounding does not matter in that case. So round up.

    // Position is always rounded to nearest.

    Rect adjustedBounds = pWindow->adjustBounds( Rect( pWindow->position(), Size(width, height) ), RoundType::nearest, RoundType::up );

    pWindow->adjustAndSetBounds(adjustedBounds);    
}

void defaultWindowCenterImpl(Window* pWindow, const Rect& screenWorkArea)
{
	Size size = pWindow->size();
    
    double x = screenWorkArea.x + (screenWorkArea.width - size.width)/2;
	double y = screenWorkArea.y + (screenWorkArea.height - size.height)/2;

    Rect newBounds( Point(x, y), size );

    pWindow->adjustAndSetBounds(newBounds);
}

}


