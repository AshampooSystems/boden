#ifndef BDN_View_H_
#define BDN_View_H_


namespace bdn
{
	class View;
}

#include <bdn/IUiProvider.h>
#include <bdn/IViewCore.h>
#include <bdn/RequireNewAlloc.h>
#include <bdn/DefaultProperty.h>

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
		initProperty<bool, IViewCore, &IViewCore::setVisible>(_visible);
	}

	~View()
	{
		// We have to manually deinit the core here (if we have one) to ensure that it is not deleted
		// from a thread other than the main thread.
		_deinitCore();
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
		// no need for mutex locking. Properties are thread-safe.
		return _visible;
	}

	virtual const ReadProperty<bool>& visible() const
	{
		// no need for mutex locking. Properties are thread-safe.
		return _visible;
	}

	
	/** Returns the UI provider used by this view. This can be nullptr if no UI provider
		is currently associated with the view object. This can happen, for example, when the
		view object is not yet connected to a top level window (either directly or indirectly).
		
		Note that there can sometimes be a short delay after a window has been added to a new parent
		until its UI provider becomes available in the child view.
		*/
	P<IUiProvider> getUiProvider()
	{
		MutexLock lock(_mutex);

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
		MutexLock lock(_mutex);

		return _pParentViewWeak;
	}


	void 

	/** Stores a list with all the child views in the target list object.*/
	virtual void getChildViews(std::list< P<View> >& childViews)=0;

protected:
	void deleteThis() override
	{
		// release all property subscriptions
		_propertySubs.clear();

		// Normally the reference count SHOULD still be the same as when the last reference was released.
		// When that happened, _deleteThisRefCountDelta was subtracted from the refcount for implementation reasons.
			
		// No one else has a reference to us, so no other thread could have called incRef.

		// The only exception to this are scheduled notifications from property changes.
		// Those do not hold a reference to us (by design).

		// IF the reference count has changed then we know that a change notification was still scheduled
		// and executed in another thread just before we deleted the subscriptions. During that change
		// notification a new reference was added.

		// In that case we need to revive the object and keep it alive until the notification has finished.

		// Note that this cannot happen after the property subs have been released. The Notifier object
		// ensures that no more notifications are called after the sub was deleted and that all notifications
		// that were in progress are done.

		if(getRefCount() > -_deleteThisRefCountDelta)
		{
			// there was an in-progress notifications and it added a new reference.

			// So we abort the deletion.
			P<IBase> pNewRef = cancelDeleteThisAndReturnNewReference();

			// Immediately release the new reference. If the pending call has finished
			// already then this will re-trigger our deletion. Otherwise the deletion will
			// re-trigger when the pending operation releases its reference.
			pNewRef = nullptr;			
		}
		else
		{
			// continue deletion
			Base::deleteThis();
		}
	}

	template<typename ValueType, class CoreInterfaceType, void (CoreInterfaceType::*Func)(const ValueType &)>
	void initProperty( Property<ValueType>& prop )
	{	
		_propertySubs.emplace_front();
		prop.onChange().subscribeMember<View>( _propertySubs.front(), this, &View::propertyChanged<ValueType, CoreInterfaceType, &IViewCore::setVisible> );
	}

	template<typename ValueType, class CoreInterfaceType, void (CoreInterfaceType::*Func)(const ValueType &) >	
	void propertyChanged(const ReadProperty<ValueType>& prop )
	{
		// note that our object is guaranteed to be fully alive during this function call.
		// That is guaranteed because we delete the change subscriptions
		// in deleteThis, before the object is deleted. And that deletion will block
		// until this call has finished.

		// get the core. Note that it is OK if the core object
		// is replaced directly after this during this call.
		// We will update an outdated core, but that should have no effect.
		// And the new core will automatically get the up-to-date value from
		// the property.
		P<CoreInterfaceType>	pCore = cast<CoreInterfaceType>( getViewCore() );
		if(pCore!=nullptr)
		{
			// keep the view object alive until the call has finished. Note that this works
			// even if deleteThis has started in another thread and is currently waiting for this
			// propertyChanged call to end. That is because we check the refCount after the subscriptions
			// are deleted and if there is a new reference then we abort the deletion.
			P<View>	pThis = this;
		
			// now schedule an update to the core from the main thread.
			callFromMainThread( [pCore, pThis, &prop](){ pCore->*Func( prop.get() ); } );
		}
	}



	void setParentView(View* pParentViewWeak)
	{
		// allocate on heap so that reinitCore can release the lock
		P<MutexLock> pLock = newObj<MutexLock>(_mutex);

		bool mustReinitCore = true;

		if(pParentViewWeak==_pParentViewWeak)
		{
			// not changed
			mustReinitCore = false;
		}
		else
		{
			_pParentViewWeak = pParentViewWeak;

			P<IUiProvider>	pNewUiProvider = _pParentViewWeak->getUiProvider();
			
			// see if we need to throw away our current core and create a new one.
			// The reason why we don't always throw this away is that the change in parents
			// might simply be a minor layout position change, and in that case the UI provider
			// might want to animate this change. To allow for that, we have to keep the old core
			// and tell it to switch parents
			// Note that we can only keep the current core if the old and new parent's
			// use the same UI provider.
			if(_pUiProvider==pNewUiProvider
				&& _pUiProvider!=nullptr
				&& _pParentViewWeak!=nullptr
				&& _pCore!=nullptr)
			{
				// we try to move the core to the new parent.	

				if(Thread::isCurrentMain())
				{
					// directly call this here. We treat this case differently
					// (rather than just always calling callFromMainThread) because
					// we need to handle the locking differently when we are already in the main thread.
					if(! _pCore->tryChangeParentView(_pParentViewWeak) )
						mustReinitCore = true;
				}
				else
				{
					// schedule the update to happen in the main thread.
					// Do not reinit the core from THIS thread.
					mustReinitCore = false;

					P<IViewCore>	pCore = _pCore;
					P<View>			pThis = this;
					P<View>			pParentView = _pParentViewWeak;	// strong reference - we need to keep this alive during the call
					asyncCallFromMainThread(
						[pThis, pCore, pParentView]()
						{
							P<MutexLock> pLock = newObj<MutexLock>( pThis->_mutex );

							if(pThis->_pCore == pCore && pThis->_pParentViewWeak==pParentView)
							{
								if(! pThis->_pCore->tryChangeParentView(pThis->_pParentViewWeak) )
									pThis->reinitCore( &pLock );
							}
					} );
				}
			}
		}

		if(mustReinitCore)
			reinitCore( &pLock );

		// note that there is no need to update the UI provider of the child views.
		// If our UI provider changed then we will reinit our core. That automatically
		// causes our child cores to be reinitialized. Which in turn updates their view provider.
	}


	/** (Re-)initializes the core object of the view. If a core object existed before then
		the old object is destroyed.

		The core object is immediately detached from the view (before reinitCore returns).
		
		If the view is part of a UI hierarchy that is connected to a top level window then a new
		core will be created shortly thereafter.

		If reinitCore is called from the main thread then a new core is immediately created and
		attached, before reinitCore returns.

		If reinitCore is called from some other thread then the core will be initially null
		when the function returns. A new core will be created asynchronously and will be set shortly
		thereafter.

		reinitCore also causes the reinitialization of the cores of all child views.

		ppPreExistingLockToRelease is an optional pointer-pointer to a lock object
		of the View's main mutex. If this is not null then the corresponding pointer
		is set to null when reinitCore wants the main mutex to be released.
		This can be used if an outer function locks the mutex for preliminary work
		and wants the core to be detached without unlocking the mutex in between
		(for example, so that no intermediate state with an outdated core is exposed).

		*/
	void reinitCore( P<MutexLock>* ppPreExistingLockToRelease = nullptr )
	{
		_deinitCore( ppPreExistingLockToRelease );

		// at this point our core and the core of our child views is null.
		// The referenced core objects might still exist for a few moments (pending
		// to be destroyed from the main thread), but they are not connected to us
		// anymore.

		// now schedule a new core to be created from the main thread. Keep ourselves alive while we do that.
		_initCore();
	}

private:
	

	/** Should not be called directly. Use reinitCore() instead.
	
		ppPreExistingLockToRelease is an optional pointer-pointer to a lock object
		of the View's main mutex. If this is not null then the corresponding pointer
		is set to null when _deinitCore wants the main mutex to be released.
		This can be used if an outer function locks the mutex for preliminary work
		and wants the core to be detached without unlocking the mutex in between
		(for example, so that no intermediate state with an outdated core is exposed).
	*/
	void _deinitCore( P<MutexLock>* ppPreExistingLockToRelease = nullptr )
	{
		// we must not release the core from another thread. So we do that from the
		// main thread.
		
		P<IViewCore>	pOldCore;

		std::list< P<View> > childViewsCopy;

		{
			MutexLock		lock(_mutex);
			
			pOldCore = getViewCore();

			_pCore = nullptr;
			_pUiProvider = nullptr;

			getChildViews( childViewsCopy );
		}

		pPreExistingLockToRelease = nullptr;

		// also release the core of all child views
		for( auto pChildView: childViewsCopy )
			pChildView->_deinitCore();

		// now schedule the core reference to be released from the main thread.
		// note that we do nothing in the scheduled function. We only use this to keep the core alive
		// and cause its final release to be called from the main thread.
		callFromMainThread( [pOldCore](){} );
	}


	/** Should not be called directly. Use reinitCore() instead.*/
	void _initCore()
	{
		P<View> pThis = this;

		callFromMainThread(
			[pThis]()
			{
				// note that there is no need to protect against race conditions here.

				// Cores are ONLY created in the main thread.
				// So if someone updates the core in between then that change will also
				// be scheduled and executed AFTER the first init. So the order is ok.
				// Also note that there are no parameters for the creation function. So
				// and other pending create will only re-do what we already did here.
				// And that is fine - the worst thing that can happen is that we get a short
				// flicker in a very rare case.

				std::list< P<View> > childViews;

				{
					MutexLock lock( pThis->_mutex );	

					if(pThis->_pParentViewWeak != nullptr)
						pThis->_pUiProvider = pThis->_pParentViewWeak->_pUiProvider;
					else
						pThis->_pUiProvider = nullptr;
				
					if(pThis->_pUiProvider!=nullptr)
						pThis->_pCore = pThis->_pUiProvider->createViewCore(getViewTypeName(), pThis);

					// copy the child view list
					getChildViews( childViewsCopy );
				}

				// reinit the child views (after the parent's mutex was unlocked).
				// Note that this is totally safe: if a child view is removed from the
				// parent in the meantime then createViewCore will not have an effect
				// (since then the choöd has no UI provider).
				// If a child is moved to a new parent then createViewCore might cause
				// a double reinit, but that should be a rare occurrence and apart
				// from a possible short flicker it will be fine.

				for(auto pChildView: childViews)
					pChildView->_initCore();
			} );		
	}



protected:
	Mutex					_mutex;

	DefaultProperty<bool>	_visible;

	P<IUiProvider>			_pUiProvider;

private:
	View*					_pParentViewWeak = nullptr;
	P<IViewCore>			_pCore;

	std::list< P<IBase> >	_propertySubs;
};

}

#endif

