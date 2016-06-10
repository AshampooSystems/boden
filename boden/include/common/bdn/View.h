#ifndef BDN_View_H_
#define BDN_View_H_


namespace bdn
{
	class View;
}

#include <bdn/IUiProvider.h>
#include <bdn/PropertyWithMainThreadDelegate.h>
#include <bdn/IViewCore.h>
#include <bdn/RequireNewAlloc.h>

namespace bdn
{

/** Views are the building blocks of the visible user interface.
    A view presents data or provides some user interface functionality.
    For example, buttons, text fields etc are all view objects.

	View objects must be allocated with newObj or new.
   */
class View : public RequireNewAlloc<Base, View>
{
public:
	View()
	{
		_pVisible = newObj< PropertyWithMainThreadDelegate<bool> >( nullptr, false );
	}

	// delete copy constructor
	View(const View& o) = delete;

	
	/** Returns the property which controls wether the view is
        visible or not.
        
        Note that a view with visible=true might still not show on
        the screen if one of its parents is invisible. In other words:
        this visible property only refers to the view itself, not
        the parent hierarchy.

		It is safe to access this from any thread.
		*/
	virtual Property<bool>& visible()
	{
		return *_pVisible;
	}

	virtual ReadProperty<bool>& visible() const
	{
		return *_pVisible;
	}

	
	/** Returns the UI provider used by this view. This can be nullptr if no UI provider
		is currently associated with the view object. This can happen, for example, when the
		view object is not yet connected to a top level window (either directly or indirectly).*/
	P<IUiProvider> getUiProvider()
	{
		return _pUiProvider;
	}


	/** Returns the type name of the view. This is a somewhat arbitrary name that is used
		in the internal implementation. It is NOT necessarily the same as the name of the
		C++ class of the view (although it is often similar).
		*/
	virtual String getViewTypeName() const=0;


	/** Returns the view's parent view. This can be null if the view was not yet
		added to a parent, or if the view is a top level window.*/
	virtual P<View> getParentView()
	{
		return _pParentViewWeak;
	}

	/** Stores a list with all the child views in the target list object.*/
	virtual void getChildViews(std::list< P<View> >& childViews)
	{
		childViews = _childViews;
	}

protected:	

	/** Sets the core object for this view. Derived classes must implement this.
		Note that the provided core object must be castable to a type that is compatible
		with this view.
		
		pCore can be nullptr if the object is not connected to a core.
		*/
	virtual void		setViewCore(IViewCore* pCore)
	{
		if(pCore==nullptr)
			_pVisible->setDelegate(nullptr);
		else
		{
			// note that it might be tempting to add a parameter here that prevents the property
			// to update the delegate with its current value. After all, the core was just created,
			// so the delegate should already have the correct value.
			// BUT it is important to note that properties can be changed from ANY thread.
			// So the property value might have changed in the short time since the core was created.
			_pVisible->setDelegate( newObj<IVisibleCore::VisibleDelegate>(pCore) );
		}
	}


	/** Returns a pointer to the core of this view. This can be nullptr if the view
		does not currently have a core associated with it. That is the case, for example,
		if */
	virtual IViewCore*	getViewCore()=0;


	void setParentView(View* pParentWeak)
	{
		if(pParentWeak!=_pParentViewWeak)
		{
			IViewCore*		pOldCore = getViewCore();
			P<IUiProvider>	pOldUiProvider = _pUiProvider;

			_pParentViewWeak = pParentWeak;
			if(_pParentViewWeak!=nullptr)
				_pUiProvider = _pParentViewWeak->getUiProvider();
			else
				_pUiProvider = nullptr;
				
			bool mustReinitCore = true;

			// see if we need to throw away our current core and create a new one.
			// The reason why we don't always throw this away is that the change in parents
			// might simply be a minor layout position change, and in that case the UI provider
			// might want to animate this change. To allow for that, we have to keep the old core
			// and tell it to switch parents
			if(_pUiProvider!=nullptr && _pParentViewWeak!=nullptr && pOldCore!=nullptr)
			{
				// we can only keep the current core if the old and new parent's
				// use the same UI provider.
				if(_pUiProvider == pOldUiProvider )
				{
					// same UI provider. So ask the core to switch to the new parent.
					if(pOldCore->tryChangeParentView(_pParentViewWeak))
					{
						// we can keep the current core.
						mustReinitCore = false;
					}
				}
			}

			if(mustReinitCore)
				reinitViewCore();
		}
	}

	/** (Re-)initializes the core object of the view. If a core object existed before then
		the old object is destroyed.

		A new core object is created if the view has a UI provider. That is the case if
		the view is part of a UI hierarchy that is connected to a top level window.

		If the view does not have a UI provider yet then reinitCore simply sets the core to nullptr.
		When the view is added to a parent with a UI provider then reinitCore will automatically
		be called again.

		reinitCore also reinitializes the cores of all child views.

		*/
	void reinitViewCore()
	{
		// keep the old core alive while we reinit. That prevents the child
		// cores from being destroyed before the child had a chance to reinit
		// its own core.
		P<IViewCore> pOldCore = getViewCore();

		if(_pUiProvider==nullptr)
			setViewCore(nullptr);
		else
			setViewCore( _pUiProvider->createViewCore(getViewTypeName(), this) );

		for(auto pChildView: _childViews)
			pChildView->reinitViewCore();
	}


	void addChildView(View* pChildView)
	{
		_childViews.push_back( pChildView );

		pChildView->setParentView(this);
	}

	void removeChildView(View* pChildView)
	{
		auto it = std::find( _childViews.begin(), _childViews.end(), pChildView );
		if(it != _childViews.end())
			_removeChildViewAtIterator(it);
	}

	void _removeChildViewAtIterator( std::list< P<View> >::iterator it )
	{	
		(*it)->setParentView(nullptr);
		_childViews.erase(it);
	}

	void removeAllChildViews()
	{
		while(!_childViews.empty())
			_removeChildViewAtIterator( _childViews.begin() );
	}


	P<IUiProvider>			_pUiProvider;

	View*					_pParentViewWeak = nullptr;
	std::list< P<View> >	_childViews;	

	P< PropertyWithMainThreadDelegate<bool> >	_pVisible;

	
};

}

#endif

