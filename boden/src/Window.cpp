#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/LayoutCoordinator.h>

namespace bdn
{

Window::Window(IUiProvider* pUiProvider)
{
	// windows are invisible by default
	_visible = false;

	_pUiProvider = (pUiProvider!=nullptr) ? pUiProvider : getDefaultUiProvider().getPtr();

	initProperty<String, IWindowCore, &IWindowCore::setTitle, (int)PropertyInfluence_::none>(_title);

	reinitCore();
}

Window::~Window()
{
    // if we have a content view, detach it from us.
    setContentView(nullptr);
}

void Window::requestAutoSize()
{
	LayoutCoordinator::get()->windowNeedsAutoSizing(this);	
}

void Window::requestCenter()
{
	// Since autosizing is asynchronous, this must also be done via
	// the LayoutCoordinator. Otherwise we might center with the old size
	// and would then autoSize afterwards.
	LayoutCoordinator::get()->windowNeedsCentering(this);
}

void Window::autoSize()
{
	verifyInMainThread("Window::autoSize");

	// lock the mutex so that our core does not change during measuring

	Rect newBounds;

	P<IViewCore> pCore = getViewCore();

	if(pCore==nullptr)
	{
		// cannot size without a core. Nothing to do.
		return;
	}

	SizingInfo mySizingInfo = sizingInfo();

	Rect screenArea = cast<IWindowCore>(pCore)->getScreenWorkArea();
    	
	double width = mySizingInfo.preferredSize.width;
	double height = mySizingInfo.preferredSize.height;
    
	if(width > screenArea.width)
	{
		// we do not fit on the screen at our preferred width.
		// So we reduce the width to the maximum allowed width.
		width = screenArea.width;

		// and then adapt the height accordingly (height might increase if we reduce the width).
		height = calcPreferredSize( Size(width, Size::componentNone()) ).height;	

		// if the height we calculated is bigger than the max height then we simply
		// cannot achieve our preferred size. We will have to make do with the max available size.
		if(height > screenArea.height)
			height = screenArea.height;
	}

	if(height > screenArea.height)
	{
		// height does not fit. Reduce it so that it fits.
		height = screenArea.height;

		// and then adapt the width accordingly.
		width = calcPreferredSize( Size( Size::componentNone(), height) ).width;	

		// if the width we calculated is bigger than the max width then we simply
		// cannot achieve our preferred size. We will have to make do with the max available size.
		if(width > screenArea.width)
			width = screenArea.width;
	}

    // we want to round the size up always. If the window does not exceed the screen size
    // then we want all our content to fit guaranteed. And if the window size previously exceeded the screen size
    // then it has been clipped to the screen size. And we assume that the screen size is a valid size
    // for the display and rounding does not matter in that case. So round up.

    // Position is always rounded to nearest.

    Rect adjustedBounds = adjustBounds( Rect( position(), Size(width, height) ), RoundType::nearest, RoundType::up );

    adjustAndSetBounds(adjustedBounds);
}

void Window::center()
{
	Size mySize = size();

	P<IViewCore> pCore = getViewCore();

	if(pCore==nullptr)
	{
		// cannot size without a core. Nothing to do.
		return;
	}

	Rect screenWorkArea = cast<IWindowCore>(pCore)->getScreenWorkArea();

    double x = screenWorkArea.x + (screenWorkArea.width - mySize.width)/2;
	double y = screenWorkArea.y + (screenWorkArea.height - mySize.height)/2;

    Rect newBounds( Point(x, y), size() );

    adjustAndSetBounds(newBounds);
}

Size Window::calcPreferredSize( const Size& availableSpace ) const
{
	// lock the mutex so that our child hierarchy or core does not change during measuring
	MutexLock lock( getHierarchyAndCoreMutex() );
	
	P<IWindowCore> pCore = cast<IWindowCore>( getViewCore() );

	if(pCore==nullptr)
	{
		// cannot calculate anything without a core.
		return Size(0,0);
	}

    Margin contentMargin;
	P<const View>	pContentView = getContentView();
	if(pContentView!=nullptr)
		contentMargin = pContentView->uiMarginToDipMargin( _pContentView->margin() );

	Margin myPadding = getDipPadding();

    // combine maxSize with availableSpace
    Size maxSize = preferredSizeMaximum();
    maxSize.applyMaximum(availableSpace);

    bool widthConstrained = std::isfinite(maxSize.width);
    bool heightConstrained = std::isfinite(maxSize.height);

	Size availableContentAreaSize( Size::none() );
	if(widthConstrained || heightConstrained)
	{
		// subtract size of window border, titlebar, etc.
		availableContentAreaSize = pCore->calcContentAreaSizeFromWindowSize(
			Size(widthConstrained ? maxSize.width : 10000,
				 heightConstrained ? maxSize.height : 10000) );

		// subtract window padding
		availableContentAreaSize-=myPadding;
		// subtract margin of content view
		availableContentAreaSize-=contentMargin;

		if(availableContentAreaSize.width<0)
			availableContentAreaSize.width = 0;
		if(availableContentAreaSize.height<0)
			availableContentAreaSize.height = 0;

		if(!widthConstrained)
			availableContentAreaSize.width = Size::componentNone();
		if(!heightConstrained)
			availableContentAreaSize.height = Size::componentNone();
	}
		
	Size contentSize;
	if(pContentView!=nullptr)
		contentSize = pContentView->calcPreferredSize( availableContentAreaSize );

	Size contentAreaSize = contentSize + myPadding + contentMargin;
	
    // the core will round up here if any adjustments for the current display are needed.
	Size preferredSize = pCore->calcWindowSizeFromContentAreaSize( contentAreaSize );


    // apply minimum size constraint (the maximum constraint has already been applied above)
    preferredSize.applyMinimum( preferredSizeMinimum() );

    // also apply the core's minimm size (the implementation defined minimum size that all windows
    // must at least have)
    preferredSize.applyMinimum( pCore->getMinimumSize() );


    // also apply the preferredSizeMaximum. We already applied it at the start to
    // take the constraint into account from the beginning, but it may be that prefSize
    // is bigger than the max here because the content window does not fit.
    // So we clip the result against the max here, because we never want it to be exceeded.
    // Note that we do NOT clip against availableSpace, because we WANT that to be exceeded
    // if the children do not fit.
    preferredSize.applyMaximum( preferredSizeMaximum() );


	return preferredSize;
}

void Window::layout()
{
	P<View>			pContentView = getContentView();
	P<IWindowCore>	pCore = cast<IWindowCore>( getViewCore() );

	if(pContentView==nullptr || pCore==nullptr)
	{
		// nothing to do.
		return;
	}
    
	// just set our content window to content area (but taking margins and padding into account).

	Rect contentBounds = pCore->getContentArea();
    			
	// subtract our padding
	contentBounds -= getDipPadding();

	// subtract the content view's margins
	contentBounds -= pContentView->uiMarginToDipMargin( pContentView->margin() );
    
    pContentView->adjustAndSetBounds( contentBounds );

	// note that we do not need to call layout on the content view.
	// If it needs to update its layout then the bounds change should have caused
	// it to schedule an update.
}


Margin Window::getDipPadding() const
{
    Margin myPadding;
    
    P<IViewCore> pCore = getViewCore();
    if(pCore!=nullptr)
    {
        // default padding is zero
        Nullable<UiMargin> pad = padding();
        if(!pad.isNull())
            myPadding = getViewCore()->uiMarginToDipMargin( pad );
    }
        
    return myPadding;
}



}


