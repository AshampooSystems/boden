#include <bdn/init.h>
#include <bdn/ScrollView.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/IScrollViewCore.h>

namespace bdn
{

ScrollView::ScrollView()
    : _verticalScrollingEnabled(true)
{
	initProperty<bool, IScrollViewCore, &IScrollViewCore::setHorizontalScrollingEnabled, (int)PropertyInfluence_::preferredSize | (int)PropertyInfluence_::childLayout>(_horizontalScrollingEnabled);
    initProperty<bool, IScrollViewCore, &IScrollViewCore::setVerticalScrollingEnabled, (int)PropertyInfluence_::preferredSize | (int)PropertyInfluence_::childLayout>(_verticalScrollingEnabled);

    initProperty<Rect, IScrollViewCore, nullptr, 0>(_visibleClientRect);
}


void ScrollView::scrollClientRectToVisible(const Rect& area)
{
	Thread::assertInMainThread();

    if( isBeingDeletedBecauseReferenceCountReachedZero() )
	{
		// this happens when invalidateSizingInfo is called during the destructor.
		// In this case we do not schedule the invalidation, since the view
		// will be gone anyway.

		// So, do nothing.
        return;
	}


    P<IScrollViewCore> pCore = cast<IScrollViewCore>( getViewCore() );
    if(pCore!=nullptr)
        pCore->scrollClientRectToVisible(area);
}

}


