#include <bdn/init.h>
#include <bdn/Window.h>


#include <bdn/LayoutCoordinator.h>


namespace bdn
{

Window::Window(IUiProvider* pUiProvider)
{
	_pUiProvider = (pUiProvider!=nullptr) ? pUiProvider : getDefaultUiProvider().getPtr();

	initProperty<String, IWindowCore, &IWindowCore::setTitle>(_title);

	reinitCore();
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
	
	int width = mySizingInfo.preferredSize.width;
	int height = mySizingInfo.preferredSize.height;

	if(width > screenArea.width)
	{
		// we do not fit on the screen at our preferred width.
		// So we reduce the width to the maximum allowed width.
		width = screenArea.width;

		// and then adapt the height accordingly (height might increase if we reduce the width).
		height = calcPreferredHeightForWidth(width);	

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
		width = calcPreferredWidthForHeight(height);	

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

	Rect screenClientRect = cast<IWindowCore>(pCore)->getScreenWorkArea();

	myBounds.x = screenClientRect.x + (screenClientRect.width - myBounds.width)/2;
	myBounds.y = screenClientRect.y + (screenClientRect.height - myBounds.height)/2;

	bounds() = myBounds;
}

Size Window::calcPreferredSize() const
{
	// lock the mutex so that our child hierarchy or core does not change during measuring
	MutexLock lock( getHierarchyAndCoreMutex() );

	Size preferredSize;

	P<IWindowCore> pCore = cast<IWindowCore>( getViewCore() );

	if(pCore!=nullptr)
	{
		P<const View> pContentView = getContentView();

		if(pContentView!=nullptr)
		{
			Margin contentMargin = pContentView->uiMarginToPixelMargin( pContentView->margin() );
			
			SizingInfo contentSizingInfo = pContentView->sizingInfo();

			preferredSize = contentSizingInfo.preferredSize + contentMargin;
		}
			
		Margin myPadding = pCore->uiMarginToPixelMargin( padding() );

		preferredSize += myPadding;

		// calculate window sizes from client sizes
		preferredSize = pCore->calcWindowSizeFromContentAreaSize( preferredSize );

		Size minSize = pCore->calcMinimumSize();
		preferredSize.width = std::max( minSize.width, preferredSize.width );
		preferredSize.height = std::max( minSize.height, preferredSize.height );
	}

	return preferredSize;
}

int Window::calcPreferredWidthForHeight(int height) const
{
	P<IWindowCore>	pCore = cast<IWindowCore>(getViewCore());
	
	if(pCore==nullptr)
	{
		// cannot calculate anything without a core.
		return 0;
	}
	
	// we do not yet know our width. So we use a very large width for the purpose of the calculation.
	int contentAreaHeight = pCore->calcContentAreaSizeFromWindowSize( Size(10000, height) ).height;
	
	Margin myPadding = pCore->uiMarginToPixelMargin( padding() );

	Margin contentMargin;
	P<const View>	pContentView = getContentView();
	if(pContentView!=nullptr)
		contentMargin = pContentView->uiMarginToPixelMargin( _pContentView->margin() );

	int contentHeight = contentAreaHeight - (myPadding.top + myPadding.bottom + contentMargin.top + contentMargin.bottom);

	int	contentWidth=0;	
	if(pContentView!=nullptr)
		contentWidth = pContentView->calcPreferredWidthForHeight(contentHeight);

	int contentAreaWidth = contentWidth + myPadding.left + myPadding.right + contentMargin.left + contentMargin.right;

	Size windowSize = pCore->calcWindowSizeFromContentAreaSize( Size(contentAreaWidth, contentAreaHeight) );

	return windowSize.width;
}


int Window::calcPreferredHeightForWidth(int width) const
{
	P<IWindowCore>	pCore = cast<IWindowCore>(getViewCore());
	
	if(pCore==nullptr)
	{
		// cannot calculate anything without a core.
		return 0;
	}

	// we do not yet know our height. So we use a very large height for the purpose of the calculation.
	int contentAreaWidth = pCore->calcContentAreaSizeFromWindowSize( Size(width, 10000) ).width;
	
	Margin myPadding = pCore->uiMarginToPixelMargin( padding() );

	Margin contentMargin;
	P<const View>	pContentView = getContentView();
	if(pContentView!=nullptr)
		contentMargin = pContentView->uiMarginToPixelMargin( pContentView->margin() );

	int contentWidth = contentAreaWidth - (myPadding.left + myPadding.right + contentMargin.left + contentMargin.right);

	int	contentHeight=0;	
	if(pContentView!=nullptr)
		contentHeight = pContentView->calcPreferredHeightForWidth(contentWidth);

	int contentAreaHeight = contentHeight + myPadding.top + myPadding.bottom + contentMargin.top + contentMargin.bottom;

	Size windowSize = pCore->calcWindowSizeFromContentAreaSize( Size(contentAreaWidth, contentAreaHeight) );

	return windowSize.height;
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
	contentBounds -= pCore->uiMarginToPixelMargin( padding() );

	// subtract the content view's margins
	contentBounds -= pContentView->uiMarginToPixelMargin( pContentView->margin() );

	pContentView->bounds() = contentBounds;

	// note that we do not need to call layout on the content view.
	// If it needs to update its layout then the bounds change should have caused
	// it to schedule an update.
}



}


