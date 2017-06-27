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
}

ScrollView::~ScrollView()
{
    // if we have a content view, detach it from us.
    setContentView(nullptr);
}

void ScrollView::layout()
{
	P<View>			    pContentView = getContentView();
	P<IScrollViewCore>	pCore = cast<IScrollViewCore>( getViewCore() );

	if(pCore!=nullptr)
    {
        // the core takes care of the layout because this is highly implementation
        // specific. The available space depends on things like the size of scroll bars,
        // whether or not scroll bars are shown and similar things.
        pCore->layout();
    }
}


}


