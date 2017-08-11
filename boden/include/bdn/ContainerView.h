#ifndef BDN_ContainerView_H_
#define BDN_ContainerView_H_

#include <bdn/View.h>
#include <bdn/ViewLayout.h>

namespace bdn
{
	

/** Base class for views that contains multiple child views.	

    Derived classes must override the calcContainerLayout() and calcContainerPreferredSize()
    methods.

	*/
class ContainerView : public View
{
public:
	ContainerView()
	{		
	}


	
	/** Static function that returns the type name for #ContainerView core objects.*/
	static String getContainerViewCoreTypeName()
	{
		return "bdn.ContainerViewCore";
	}

	String getCoreTypeName() const override
	{
		return getContainerViewCoreTypeName();
	}



	/** Adds a child to the end of the container.
	
		If the child view is already a child of this container then it
		is moved to the end.
	*/
	void addChildView(View* pChildView)
	{
		insertChildView(nullptr, pChildView);
	}


	/** Inserts a child before another child.
	
		If pInsertBeforeChildView is nullptr then the new view is added to the end of
		the container.

		If pInsertBeforeChildView is not a child of this container then the new child
		view is added to the end of the container.		

		If the child view is already a child of this container then it
		is moved to the desired target position.
		*/
	void insertChildView(View* pInsertBeforeChildView, View* pChildView)
	{
		// we use a single global mutex to synchronize changes to parent-child relationships.
		// See getHierarchyMutex() for more info.
		MutexLock lock( getHierarchyAndCoreMutex() );

		P<View> pOldParentView = pChildView->getParentView();
		if(pOldParentView!=nullptr)
		{
			// do not use removeChildView on the old parent. Instead we use
			// childViewStolen. The difference is that with childViewStolen the
			// old parent will NOT call setParentView on its old child.
			// That is what we want since we want a single call to setParentView
			// at the end of the whole operation, so that the core can potentially
			// be moved directly to its new parent, without being destroyed and
			// recreated.
			pOldParentView->_childViewStolen(pChildView);
		}

		std::list< P<View> >::iterator it;
		if(pInsertBeforeChildView==nullptr)
			it = _childViews.end();
		else		
			it = std::find( _childViews.begin(), _childViews.end(), pInsertBeforeChildView );

		_childViews.insert(it, pChildView);

		pChildView->_setParentView(this);

        // the child will schedule a sizing info update for us when it gets its core.
	}


	/** Removes the specified child view from the container.

		Has no effect if the specified view is not currently a child of this container.
	*/
	void removeChildView(View* pChildView)
	{
		// we use a single global mutex to synchronize changes to parent-child relationships.
		// See getHierarchyMutex() for more info.
		MutexLock lock( getHierarchyAndCoreMutex() );

		auto it = std::find( _childViews.begin(), _childViews.end(), pChildView );
		if(it!=_childViews.end())
		{
			_childViews.erase(it);
			pChildView->_setParentView(nullptr);
		}
	}

    void removeAllChildViews() override
    {
        MutexLock lock( getHierarchyAndCoreMutex() );

        for(auto& pChildView: _childViews)            
			pChildView->_setParentView(nullptr);

        _childViews.clear();
    }

	void getChildViews(std::list< P<View> >& childViews) const override
	{
		// we use a single global mutex to synchronize changes to parent-child relationships.
		// See getHierarchyMutex() for more info.
		MutexLock lock( getHierarchyAndCoreMutex() );

		childViews = _childViews;
	}



	P<View> findPreviousChildView(View* pChildView) override
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		View* pPrevChildView = nullptr;
		for( const P<View>& pCurrView: _childViews)
		{
			if(pCurrView.getPtr()==pChildView)
				return pPrevChildView;

			pPrevChildView = pCurrView;			
		}

		return nullptr;		
	}


	void _childViewStolen(View* pChildView) override
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		auto it = std::find( _childViews.begin(), _childViews.end(), pChildView);
		if(it!=_childViews.end())
			_childViews.erase(it);
	}

	
    /** Calculates the layout for the container based on the specified total container size.
        The sizes and positions of the child views are calculated and stored in the returned
        layout object. The layout can then be applied later by calling ViewLayout::applyTo().

        calcContainerLayout may be called multiple times to create multiple layout objects
		for different sizes.
        Any of the created layouts may be applied later with ViewLayout::applyTo(). It is
        also valid to apply none of them and throw them away.

		\param containerSize the size of the container view to use as the basis for the layout.
			This does not have to match the current size of the container view.			
        */
    virtual P<ViewLayout> calcContainerLayout(const Size& containerSize) const=0;


    /** Calculates the preferred size for the container. Container implementations must override
        this to implement their custom size calculation.

        calcContainerPreferredSize is automatically called from View::calcPreferredSize()
        and has the same behaviour.        

        */
    virtual Size calcContainerPreferredSize( const Size& availableSpace = Size::none() ) const=0;

	
protected:
	std::list< P<View> > _childViews;
};


}

#endif