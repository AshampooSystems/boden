#ifndef BDN_ScrollView_H_
#define BDN_ScrollView_H_

#include <bdn/View.h>

#include <bdn/IScrollViewCore.h>

namespace bdn
{

/** A window with scrollable content.	

	ScrollViews have a single child view (the "content view") that displays the scrollable
    contents. Usually this content view will be a view container object,
	which can then contain multiple child views.

    Scroll views scroll only vertically by default. See horizontalScrollingEnabled()
    and verticalScrollingEnabled() to change that.

    Platform note:

    When you are building a web application (i.e. you are building for the webems platform)
    then you should be aware that some browsers do not show scrollbars when the ScrollView
    is too small. What this minimum size for scrolling is depends on the browser AND the
    host operating system. For example, at the time of this writing, Firefox on Mac shows
    scrollbars when the ScrollView viewport is at least 12 pixels wide/high, But Firefox
    on Linux has a higher limit and only shows scrollbars when the ScrollView viewport is
    at least 42 pixels.
    Note that these numbers are only examples - you should never depend on them. The important
    thing to remember is that on the webems platform you might not get scrollbars when the
    ScrollView is very small.
*/
class ScrollView : public View
{
public:
	ScrollView();

    /** Controls wether or not the view scrolls vertically.
        Default: true*/
    BDN_VIEW_PROPERTY( bool, verticalScrollingEnabled, setVerticalScrollingEnabled, IScrollViewCore, influencesPreferredSize().influencesContentLayout() );

    /** Controls wether or not the view scrolls horizontally.
        Default: false*/
    BDN_VIEW_PROPERTY(bool, horizontalScrollingEnabled, setHorizontalScrollingEnabled, IScrollViewCore, influencesPreferredSize().influencesContentLayout() );

    /** Read-only property that indicates the part of the client area (=the scrolled area) that
        is currently visible. The rect is in client coordinates (see \ref layout_box_model.bd).
                
        The top-left corner of the visible client rect is what is also commonly referred to as the 
        "scroll position".
        
        The visible client rect / scroll position can be manipulated with scrollClientRectToVisible().
    */
    BDN_VIEW_PROPERTY_WITHOUT_CORE_FORWARDING(Rect, visibleClientRect, _setVisibleClientRect, influencesNothing() );
    // note that we do not forward changes to the visibleClientRect to the core, because this property is updated BY
    // the core. Also note that we set the influences to nothing because the corresponding layouting invalidations are
    // already triggered by the process that has caused this change.
    
    /** \fn void ScrollView::_setVisibleClientRect(const Rect&)
     
        Called by the scroll view's core when the visible client area
        changes. This updates the value of the visibleClientRect property.

        This should only be called by the core. In particular, the application
        should never call this. scrollClientRectToVisible() can be used
        to manipulate the scroll position.
        */


	/** Sets the specified view as the content view of the window.
		Note that ScrollView object can only have a single child content view. If one is already
		set then it will be replaced.
		See #ScrollView class documentation for more information.*/
	virtual void setContentView(View* pContentView)
	{
		Thread::assertInMainThread();

		if(pContentView!=_pContentView)
		{
			if(_pContentView!=nullptr)
				_pContentView->_setParentView(nullptr);

			_pContentView = pContentView;

            if(_pContentView!=nullptr)
			    _pContentView->_setParentView(this);
            
            invalidateSizingInfo( View::InvalidateReason::childAddedOrRemoved );
            needLayout( View::InvalidateReason::childAddedOrRemoved );
		}
	}


	/** Returns the scroll view's content view (see #getContentView()).
		This can be nullptr if no content view has been set yet.*/
	virtual P<View> getContentView()
	{
		Thread::assertInMainThread();

		return _pContentView;
	}


	/** Returns the scroll view's content view (see #getContentView()).
		This can be nullptr if no content view has been set yet.*/
	virtual P<const View> getContentView() const
	{
		Thread::assertInMainThread();

		return _pContentView;
	}



    /** Changes the scroll position so that the specified part of the
        client area is visible.

        The specified rect is in client coordinates (see \ref layout_box_model.md).

        It is ok to specify coordinates outside the client area. If the rect exceeds
        the client area then the function will make sure that the edge of the client area
        in that direction is visible.
        
        If the specified target rect is bigger than the viewport then of course not all
        of the rect can be visible. In this case the scroll view will scroll the minimal amount
        possible, to make as much of the target rect visible as possible. For example, if the scroll position
        was previously to the right of the target rect then it will scroll so that the right part of the target
        rect is visible. The left part of the target rect will be out of view, since it does not fit
        into the viewport. If the scroll position before the call had been to the left of the target rect
        then after the call the left part of the target rect would be visible and the right part
        would be out of view.

        scrollClientRectToVisible also supports the special "infinity" floating point values.
        If positive infinity is specified as the rect position then the function will scroll
        to the end of the client area in that direction.
        A negative infinity position scrolls to the start, although you can of course also
        simply specify 0 instead.
        */
    virtual void scrollClientRectToVisible(const Rect& clientRect);
	

	/** Static function that returns the type name for #ScrollView objects.*/
	static String getScrollViewCoreTypeName()
	{
		return "bdn.ScrollView";
	}

	String getCoreTypeName() const override
	{
		return getScrollViewCoreTypeName();
	}


	void getChildViews( List< P<View> >& childViews) const override
	{
		Thread::assertInMainThread();

		if(_pContentView!=nullptr)
			childViews.push_back(_pContentView);	
	}


    void removeAllChildViews() override
    {
        setContentView(nullptr);
    }
		
	P<View> findPreviousChildView(View* pChildView) override
	{
		// we do not have multiple child views with an order - just a single content view
		return nullptr;
	}


	void _childViewStolen(View* pChildView) override
	{
		Thread::assertInMainThread();

		if(pChildView==_pContentView)
			_pContentView = nullptr;
	}
   
    
private:
    P<View>		   _pContentView;
};

}

#endif

