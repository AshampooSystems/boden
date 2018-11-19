#include <bdn/init.h>
#include <bdn/ScrollView.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/IScrollViewCore.h>

namespace bdn
{

    ScrollView::ScrollView() { setVerticalScrollingEnabled(true); }

    void ScrollView::scrollClientRectToVisible(const Rect &area)
    {
        Thread::assertInMainThread();

        if (isBeingDeletedBecauseReferenceCountReachedZero()) {
            // this happens when invalidateSizingInfo is called during the
            // destructor. In this case we do not schedule the invalidation,
            // since the view will be gone anyway.

            // So, do nothing.
            return;
        }

        P<IScrollViewCore> core = cast<IScrollViewCore>(getViewCore());
        if (core != nullptr)
            core->scrollClientRectToVisible(area);
    }
}
