#include <bdn/init.h>
#include <bdn/ScrollView.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/IScrollViewCore.h>

namespace bdn
{

ScrollView::ScrollView()
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

	if(pContentView==nullptr || pCore==nullptr)
	{
		// nothing to do.
		return;
	}

    bool scrollHorz = _horizontalScrollingEnabled;
    bool scrollVert = _verticalScrollingEnabled;

    Size prefSize = pContentView->sizingInfo().get().preferredSize;
    Size viewPortSize = pCore->getViewPortSize();
    Size contentSize;
    
    if(scrollHorz)
        contentSize.width = prefSize.width;
    else
        contentSize.width = viewPortSize.width;

    if(scrollVert)
        contentSize.height = prefSize.height;
    else
        contentSize.height = viewPortSize.height;


    
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


Margin ScrollView::getDipPadding() const
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


