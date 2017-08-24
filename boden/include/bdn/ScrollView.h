#ifndef BDN_ScrollView_H_
#define BDN_ScrollView_H_

#include <bdn/View.h>

namespace bdn
{

/** A window with scrollable content.	

	ScrollViews have a single child view (the "content view") that displays the scrollable
    contents. Usually this content view will be a view container object,
	which can then contain multiple child views.

    Scroll views scroll only vertically by default. See horizontalScrollingEnabled()
    and verticalScrollingEnabled() to change that.
*/
class ScrollView : public View
{
public:
	ScrollView();

    /** Controls wether or not the view scrolls vertically.
        Default: true*/
    virtual Property<bool>& verticalScrollingEnabled()
    {
        return _verticalScrollingEnabled;
    }

    virtual const ReadProperty<bool>& verticalScrollingEnabled() const
    {
        return _verticalScrollingEnabled;
    }


    /** Controls wether or not the view scrolls horizontally.
        Default: false*/
    virtual Property<bool>& horizontalScrollingEnabled()
    {
        return _horizontalScrollingEnabled;
    }

    virtual const ReadProperty<bool>& horizontalScrollingEnabled() const
    {
        return _horizontalScrollingEnabled;
    }
     


	/** Sets the specified view as the content view of the window.
		Note that ScrollView object can only have a single child content view. If one is already
		set then it will be replaced.
		See #ScrollView class documentation for more information.*/
	virtual void setContentView(View* pContentView)
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

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
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pContentView;
	}


	/** Returns the scroll view's content view (see #getContentView()).
		This can be nullptr if no content view has been set yet.*/
	virtual P<const View> getContentView() const
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pContentView;
	}



    /** Changes the scroll position so that the specified area is visible.
        The specified rect coordinates refer to scroll view's inner scrolled
        coordinate system. I.e. The point (0,0) refers to the top left corner
        of the scrolled area, which is not necessarily the same
        of the top-left corner the content view (if the content view has a nonzero margin,
        or if the scroll view has nonzero padding).

        This function is thread safe.
        */
    virtual void scrollAreaToVisible(const Rect& area);
	

	/** Static function that returns the type name for #ScrollView objects.*/
	static String getScrollViewCoreTypeName()
	{
		return "bdn.ScrollView";
	}

	String getCoreTypeName() const override
	{
		return getScrollViewCoreTypeName();
	}


	void getChildViews(std::list< P<View> >& childViews) const override
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

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
		MutexLock lock( getHierarchyAndCoreMutex() );

		if(pChildView==_pContentView)
			_pContentView = nullptr;
	}
    
private:
    P<View>					_pContentView;

    DefaultProperty<bool>   _verticalScrollingEnabled;
    DefaultProperty<bool>   _horizontalScrollingEnabled;
};

}

#endif

