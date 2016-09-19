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
		height = calcPreferredSize(width).height;	

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
		width = calcPreferredSize(-1, height).width;	

		// if the width we calculated is bigger than the max width then we simply
		// cannot achieve our preferred size. We will have to make do with the max available size.
		if(width > screenArea.width)
			width = screenArea.width;
	}

	newBounds = bounds();
	newBounds.width = width;
	newBounds.height = height;

	bounds() = newBounds;
}

void Window::center()
{
	Rect myBounds = bounds();

	P<IViewCore> pCore = getViewCore();

	if(pCore==nullptr)
	{
		// cannot size without a core. Nothing to do.
		return;
	}

	Rect screenWorkArea = cast<IWindowCore>(pCore)->getScreenWorkArea();

	myBounds.x = screenWorkArea.x + (screenWorkArea.width - myBounds.width)/2;
	myBounds.y = screenWorkArea.y + (screenWorkArea.height - myBounds.height)/2;

	bounds() = myBounds;
}

Size Window::calcPreferredSize(double availableWidth, double availableHeight) const
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

	Size availableContentAreaSize(-1, -1);	
	if(availableWidth!=-1 || availableHeight!=-1)
	{
		// subtract size of window border, titlebar, etc.
		availableContentAreaSize = pCore->calcContentAreaSizeFromWindowSize(
			Size(availableWidth==-1 ? 10000 : availableWidth,
				 availableHeight==-1 ? 10000 : availableHeight) );

		// subtract window padding
		availableContentAreaSize-=myPadding;
		// subtract margin of content view
		availableContentAreaSize-=contentMargin;

		if(availableContentAreaSize.width<0)
			availableContentAreaSize.width = 0;
		if(availableContentAreaSize.height<0)
			availableContentAreaSize.height = 0;

		if(availableWidth==-1)
			availableContentAreaSize.width = -1;
		if(availableHeight==-1)
			availableContentAreaSize.height = -1;
	}
		
	Size contentSize;
	if(pContentView!=nullptr)
		contentSize = pContentView->calcPreferredSize( availableContentAreaSize.width, availableContentAreaSize.height );

	Size contentAreaSize = contentSize + myPadding + contentMargin;
	
	Size preferredSize = pCore->calcWindowSizeFromContentAreaSize( contentAreaSize );

	Size minSize = pCore->calcMinimumSize();
	preferredSize.width = std::max( minSize.width, preferredSize.width );
	preferredSize.height = std::max( minSize.height, preferredSize.height );

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
    
    
	pContentView->bounds() = contentBounds;

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


