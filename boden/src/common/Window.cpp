#include <bdn/init.h>
#include <bdn/Window.h>


namespace bdn
{


void Window::updateSizingInfo()
{
	MutexLock lock( getHierarchyAndCoreMutex() );

	LayoutSizingInfo sizingInfo;

	if(_pCore!=nullptr)
	{
		P<View> pContentView = cast<Window>(_pOuterViewWeak)->getContentView();

		if(pContentView!=nullptr)
		{
			Margin contentMargin = pContentView->uiMarginToPixelMargin( pContentView->margin() );
			
			pContentView->getSizingInfo(sizingInfo);

			sizingInfo.minSizeWithSmallestWidth += contentMargin;
			sizingInfo.minSizeWithSmallestHeight += contentMargin;
			sizingInfo.preferredSize += contentMargin;
		}
			
		Margin padding = _pCore->uiMarginToPixelMargin( padding() );

		sizingInfo.minSizeWithSmallestWidth += padding;
		sizingInfo.minSizeWithSmallestHeight += padding;
		sizingInfo.preferredSize += padding;

		// calculate window sizes from client sizes
		sizingInfo.minSizeWithSmallestWidth = _pCore->getWindowSizeFromClientSize( sizingInfo.minSizeWithSmallestWidth );
		sizingInfo.minSizeWithSmallestHeight = _pCore->getWindowSizeFromClientSize( sizingInfo.minSizeWithSmallestHeight );
		sizingInfo.preferredSize = _pCore->getWindowSizeFromClientSize( sizingInfo.preferredSize );
	}

	setSizingInfo( sizingInfo );
}


void Window::layout()
{
	// this is only called from the main thread, so it is safe to access the core directly
	// here.

	MutexLock lock( getHierarchyAndCoreMutex() );

	P<View> pContentView = cast<Window>(_pOuterViewWeak)->getContentView();

	if(pContentView==nullptr || _pCore==nullptr)
	{
		// nothing to do.
		return;
	}

	// just set our content window to content area (but taking margins and padding into account).

	Rect contentViewRect = _pCore->getContentArea();
			
	// subtract our padding
	contentViewRect -= _pCore->uiMarginToPixelMargin( padding() );

	// subtract the content view's margins
	contentViewRect -= pContentView->uiMarginToPixelMargin( pContentView->margin() );

	pContentView->bounds() = contentViewRect;

	// note that we do not need to call layout on the content view.
	// If it needs to update its layout then the bounds change should have caused
	// it to schedule an update.
}



}


