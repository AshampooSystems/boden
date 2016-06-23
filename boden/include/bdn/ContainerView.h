#ifndef BDN_ContainerView_H_
#define BDN_ContainerView_H_

#include <bdn/View.h>

namespace bdn
{
	

/** Base class for views that contains multiple child views.	
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

protected:
	std::list< P<View> > _childViews;
};


}

#endif