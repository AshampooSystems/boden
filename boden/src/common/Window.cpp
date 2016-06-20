#include <bdn/init.h>
#include <bdn/Window.h>


#include <bdn/LayoutCoordinator.h>


namespace bdn
{


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

	Rect screenClientRect = pCore->getScreenClientRect();
	
	int width = mySizingInfo.preferredSize.width;
	int height = mySizingInfo.preferredSize.height;

	if(width > screenClientRect.width)
	{
		// we do not fit on the screen at our preferred width.
		// So we reduce the width to the maximum allowed width.
		width = screenClientRect.width;

		// and then adapt the height accordingly (height might increase if we reduce the width).
		height = calcPreferredHeightForWidth(width);	

		// if the height we calculated is bigger than the max height then we simply
		// cannot achieve our preferred size. We will have to make do with the max available size.
		if(height > screenClientRect.height)
			height = screenClientRect.height;
	}

	if(height > screenClientRect.height)
	{
		// height does not fit. Reduce it so that it fits.
		height = screenClientRect.height;

		// and then adapt the width accordingly.
		width = calcPreferredWidthForHeight(height);	

		// if the width we calculated is bigger than the max width then we simply
		// cannot achieve our preferred size. We will have to make do with the max available size.
		if(width > screenClientRect.width)
			width = screenClientRect.width;
	}

	newBounds = bounds();
	newBounds.width = width;
	newBounds.height = height;

	bounds() = newBounds;
}

void Window::center()
{
	Rect bounds = bounds();

	P<IViewCore> pCore = getViewCore();

	if(pCore==nullptr)
	{
		// cannot size without a core. Nothing to do.
		return;
	}

	Rect screenClientRect = pCore->getScreenClientRect();

	bounds.x = screenClientRect.x + (screenClientRect.width - bounds.width)/2;
	bounds.y = screenClientRect.y + (screenClientRect.height - bounds.height)/2;

	bounds() = bounds;
}

Size Window::calcPreferredSize() const
{
	// lock the mutex so that our child hierarchy or core does not change during measuring
	MutexLock lock( getHierarchyAndCoreMutex() );

	Size preferredSize;

	if(_pCore!=nullptr)
	{
		P<View> pContentView = cast<Window>(_pOuterViewWeak)->getContentView();

		if(pContentView!=nullptr)
		{
			Margin contentMargin = pContentView->uiMarginToPixelMargin( pContentView->margin() );
			
			SizingInfo contentSizingInfo = pContentView->sizingInfo();

			preferredSize = contentSizingInfo.preferredSize + contentMargin;
		}
			
		Margin padding = _pCore->uiMarginToPixelMargin( padding() );

		preferredSize += padding;

		// calculate window sizes from client sizes
		preferredSize = _pCore->getWindowSizeFromClientSize( preferredSize );
	}

	return preferredSize;
}

int Window::calcPreferredWidthForHeight(int height) const
{
	int clientHeight = _pCore->getClientHeightFromHeight(height);
	
	Margin padding = _pCore->uiMarginToPixelMargin( padding() );
	Margin margin = _pContentView->uiMarginToPixelMargin( _pContentView->margin() );

	int contentHeight = clientHeight - (padding.top + padding.bottom + margin.top + margin.bottom);

	int contentWidth = pContentView->calcPreferredWidthFromHeight(contentHeight);

	int clientWidth = contentWidth + padding.left + padding.right + margin.left + margin.right;

	int width = _pCore->getWidthFromClientWidth(clientWidth);

	return width;
}


int Window::calcPreferredHeightForWidth(int width) const
{
	int clientWidth = _pCore->getClientWidthFromWidth(width);
	
	Margin padding = _pCore->uiMarginToPixelMargin( padding() );
	Margin margin = _pContentView->uiMarginToPixelMargin( _pContentView->margin() );

	int contentWidth = clientWidth - (padding.left + padding.right + margin.left + margin.right);

	int contentHeight = pContentView->calcPreferredWidthFromHeight(contentWidth);

	int clientHeight = contentHeight + padding.top + padding.bottom + margin.top + margin.bottom;

	int height = _pCore->getHeightFromClientHeight(clientHeight);

	return height;
}

void Window::layout()
{
	P<View> pContentView;
	Rect    contentBounds;

	{
		// lock the mutex so that our child hierarchy or core does not change during measuring
		MutexLock lock( getHierarchyAndCoreMutex() );

		P<View> pContentView = cast<Window>(_pOuterViewWeak)->getContentView();

		if(pContentView==nullptr || _pCore==nullptr)
		{
			// nothing to do.
			return;
		}

		// just set our content window to content area (but taking margins and padding into account).

		Rect contentBounds = _pCore->getContentArea();
			
		// subtract our padding
		contentBounds -= _pCore->uiMarginToPixelMargin( padding() );

		// subtract the content view's margins
		contentBounds -= pContentView->uiMarginToPixelMargin( pContentView->margin() );
	}

	pContentView->bounds() = contentBounds;

	// note that we do not need to call layout on the content view.
	// If it needs to update its layout then the bounds change should have caused
	// it to schedule an update.
}



}


