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
#include <bdn/UiMargin.h>
#include <bdn/Rect.h>
#include <bdn/mainThread.h>

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

	View();
	~View();

	// delete copy constructor
	View(const View& o) = delete;



	/** Returns the core object of this view.

		The core can be null if the view is not currently connected (directly or indirectly) to a top level window.
		It can also be null for short periods of time when a reinitialization was necessary.

		The core provides the actual implementation of the view. It is provided by the
		IUiProvider object that the view uses. The IUiProvider is inherited from the parent view
		and can be explicitly set when creating a top level window.
		*/
	P<IViewCore> getViewCore() const
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pCore;		
	}

	
	/** Returns the property which controls wether the view is
        visible or not.
        
        Note that a view with visible=true might still not show on
        the screen if one of its parents is invisible. In other words:
        this visible property only raefers to the view itself, not
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


	/** The size of the empty space that should be left around the view.
	
		The margin is NOT part of the view itself. It is merely something that the
		layout takes into account.

		It is recommended to specify the margin in UiLength::sem units.
	*/
	virtual Property<UiMargin>& margin()
	{
		return _margin;
	}

	virtual const ReadProperty<UiMargin>& margin() const
	{
		return _margin;
	}


	/** The size space around the content inside this view.

		The padding is part of the view and thus it influences the size of
		the view (in contrast to the margin(), which is NOT part of the view).

		It is recommended to specify the padding in UiLength::sem units.
	*/
	virtual Property<UiMargin>& padding()
	{
		return _padding;
	}

	virtual const ReadProperty<UiMargin>& padding() const
	{
		return _padding;
	}


	/** Bounding rectangle of the view (size and position).*/
	virtual Property<Rect>& bounds()
	{
		return _bounds;
	}

	virtual const ReadProperty<Rect>& bounds() const
	{
		return _bounds;
	}

	

	enum class HorizontalAlignment
	{
		expand,
		left,
		center,
		right		
	};

	enum class VerticalAlignment
	{
		expand,
		top,
		bottom,
		middle
	};


	virtual Property<VerticalAlignment>& verticalAlignment()
	{
		return _verticalAlignment;
	}

	virtual const ReadProperty<VerticalAlignment>& verticalAlignment() const
	{
		return _verticalAlignment;
	}


	virtual Property<HorizontalAlignment>& horizontalAlignment()
	{
		return _horizontalAlignment;
	}

	virtual const ReadProperty<HorizontalAlignment>& horizontalAlignment() const
	{
		return _horizontalAlignment;
	}


	/*

	virtual Property<double>& extraSpaceWeight()
	{
		return _extraSpaceWeight;
	}

	virtual const ReadProperty<double>& extraSpaceWeight() const
	{
		return _extraSpaceWeight;
	}

	*/
	
	/** Returns the UI provider used by this view. This can be nullptr if no UI provider
		is currently associated with the view object. This can happen, for example, when the
		view object is not yet connected to a top level window (either directly or indirectly).
		
		Note that there can sometimes be a short delay after a window has been added to a new parent
		until its UI provider becomes available in the child view.
		*/
	P<IUiProvider> getUiProvider()
	{
		// the UI provider depends on the hierarchy
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pUiProvider;
	}


	/** Returns the type name of the view core. This is a somewhat arbitrary name that is used
		in the internal implementation. It is NOT necessarily the same as the name of the
		C++ class of the view or view core (although it is often similar).
		*/
	virtual String getCoreTypeName() const=0;
	

	/** Returns the view's parent view. This can be null if the view was not yet
		added to a parent, or if the view is a top level window.*/
	virtual P<View> getParentView()
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pParentViewWeak;
	}

	
	/** Stores a list with all the child views in the target list object.*/
	virtual void getChildViews(std::list< P<View> >& childViews) const
	{
		// no child views by default. So nothing to do.
	}


	/** Finds the child view that "precedes" the specified one.
		Returns nullptr if any of the following conditions are true:
		
		- the specified view is not a child of this view
		- the specified view is the first child of this view
		- this view does not define an order among its children

		*/
	virtual P<View> findPreviousChildView(View* pChildView)
	{
		// no child views by default
		return nullptr;
	}
	

	/** Should only be called by view container implementors when they add or remove a child.
		Users of View objects should NOT call this.		

		Tells the view object that it has a new parent.
		pParentView can be nullptr if the view was removed from a parent
		and does not currently have one.

		Note that any modifications to the view hierarchy should only be done while
		the mutex returned by getHierarchyAndCoreMutex() is locked.
		*/
	void _setParentView(View* pParentView);



	/** Should only be called by view container implementations.
		Users of View objects should NOT call this.
		
		This must be called when another view container "steals" a view that
		was formerly a child of this view.		

		Note that any modifications to the view hierarchy should only be done while
		the mutex returned by getHierarchyAndCoreMutex() is locked.
		*/
	virtual void _childViewStolen(View* pChildView)
	{
		// do nothing by default.		
	}


	/** Requests that the view updates its sizing information (preferred size, etc.).
		The measuring does not happen immediately in this function - it is performed asynchronously.
		
		Note that it is usually NOT necessary to call this as a user of a view object. The view object
		will automatically schedule re-measuring when its parameters and properties change.
		*/
	void needSizingInfoUpdate();


	/** Requests that the view updates the layout of its child view and contents.
		
		The layout operation does not happen immediately in this function - it is performed asynchronously.
		
		Note that it is usually NOT necessary to call this as a user of a view object. The view object
		will automatically schedule re-layout operations when its layout parameters or child views change.
		*/
	void needLayout();


	struct SizingInfo
	{
		Size preferredSize;

		bool operator==(const SizingInfo& o) const
		{
			return preferredSize == o.preferredSize;
		}

		bool operator!=(const SizingInfo& o) const
		{
			return !operator==(o);
		}
	};


	/** Returns the sizing information of the view (to use during layout).
	*/
	const ReadProperty<SizingInfo>& sizingInfo() const
	{
		return _sizingInfo;
	}



	/** Converts a UiMargin object to a pixel based margin object.
		This uses view-specific internal data, so the result can be different
		for different view objects.
		The result can when called again at a later time with the same view object
		(if the view's parameters or the operating systems settings have changed).

		IMPORTANT: This function must be called from the main thread.
		*/
	Margin uiMarginToPixelMargin( const UiMargin& uiMargin) const;



	

	/** Asks the view to calculate its preferred size, based on it current content
		and properties.

		Custom view implementations should override this and provide an implementation
		suitable for their content and/or child views.

		IMPORTANT: This function must only called be called from the main thread.
		*/	
	virtual Size calcPreferredSize() const;

	
	/** Asks the view to calculate its preferred height for the case that the view had
		the specified width.

		This function is called in cases when there is not enough space to size the view
		according to its unconstrained preferred size (see #calcPreferredSize()).
		The view should pretend that it has the specified width and return its preferred
		height for that case.

		Note that the \c width parameter can also be BIGGER than the unconstrained preferred width
		returned by calcPreferredSize().

		Custom view implementations should override this and provide an implementation
		suitable for their content and/or child views.
		
		IMPORTANT: This function must only be called from the main thread.
		*/	
	virtual int calcPreferredHeightForWidth(int width) const;


	/** Asks the view to calculate its preferred width for the case that the view had
		the specified height.

		This function is called in cases when there is not enough space to size the view
		according to its unconstrained preferred size (see #calcPreferredSize()).
		The view should pretend that it has the specified height and return its preferred
		width for that case.

		Note that the \c height parameter can also be BIGGER than the unconstrained preferred height
		returned by calcPreferredSize().

		Custom view implementations should override this and provide an implementation
		suitable for their content and/or child views.
		
		IMPORTANT: This function must only be called from the main thread.
		*/	
	virtual int calcPreferredWidthForHeight(int height) const;
	

protected:

	/** Verifies that the current thread is the main thread.
		Throws a ProgrammingError if that is not the case.
		The methodName parameter should be the name of the method that was called
		that should have been called from the main thread.*/
	void verifyInMainThread(const String& methodName) const;


	/** Tells the view to update its sizing info.
	
		IMPORTANT: This must only be called from the main thread.
	*/
	void updateSizingInfo();



	/**	Tells the view to update the layout of its child views. The
		view should NOT update its own size or position during this - 
		it has to work with the size and position it currently has and
		should ONLY update the size and position of its child views.
		
		IMPORTANT: This function must only be called from the main thread.
		*/
	virtual void layout()=0;


	// allow the coordinator to call the sizing info and layout functions.
	friend class LayoutCoordinator;



	/** Returns the global mutex object that is used to synchronize changes in the
		UI hierarchy (parent-child relationships) and replacement of view core objects.

		The reason why we use a single global mutex for hierarchy changes is that otherwise deadlocks
		could occur. We need to lock the old parent, the child and the new parent.
		If multiple changes with the same objects are done in different threads
		then it could potentially happen that the same two objects are locked in inverse order
		in two threads, creating a deadlock.
		For example, consider this UI hierarchy:

		A
		  B
			C
		D

		Lets say we want to move B to D and C to D at the same time. Since B is the child-to-be-moved
		for one operation and the old parent for another, the locking order could easily be inverse
		and thus a deadlock could occur.

		To avoid all this we use a single mutex for all hierarchy modifications. The impact on parallel
		performance should be negligible, since the operations are short (just setting a parent pointer or
		adding to a child list). Also, it should be a rare case when the hierarchy is modified from two
		threads at the same time.

		The same mutex is used to guard changes to the view cores. The reason is that hierarchy changes
		sometimes cause creation, destruction or replacement of view cores. And these changes can also
		propagate down the UI hierarchy (if a parent core is destroyed then all child cores must also
		be destroyed). Because of this, the hierarchy mutex must be locked whenever a core is updated
		(so that it does not change during the update operation). And if we had multiple mutexes
		for cores and the hierarchy, then such operations would again be very sensitive to locking order
		and could create potential deadlocks.
		*/
	static Mutex& getHierarchyAndCoreMutex()
	{
		static SafeInit<Mutex> init;

		return *init.get();
	}


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
		prop.onChange().subscribeMember<View>( _propertySubs.front(), this, &View::propertyChanged<ValueType, CoreInterfaceType, Func> );
	}

	template<typename ValueType, class CoreInterfaceType, void (CoreInterfaceType::*Func)(const ValueType&) >	
	void propertyChanged(const ReadProperty<ValueType>& prop )
	{
		// note that our object is guaranteed to be fully alive during this function call.
		// That is guaranteed because we delete the change subscriptions
		// in deleteThis, before the object is deleted. And that deletion will block
		// until this call has finished.

		// get the core. Note that it is OK if the core object
		// is replaced directly after this during this call.
		// We will update an outdated core, but thats should have no effect.
		// And the new core will automatically get the up-to-date value from
		// the property.
		P<CoreInterfaceType>	pCore = cast<CoreInterfaceType>( _pCore );
		if(pCore!=nullptr)
		{
			// keep the view object alive until the call has finished. Note that this works
			// even if deleteThis has started in another thread and is currently waiting for this
			// propertyChanged call to end. That is because we check the refCount after the subscriptions
			// are deleted and if there is a new reference then we abort the deletion.
			P<View>	pThis = this;
		
			// now schedule an update to the core from the main thread.
			callFromMainThread(
				[pCore, pThis, &prop]()
				{	
					(pCore->*Func)( prop.get() );
				} );
		}
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
		*/
	void reinitCore();


	/** Determines the Ui provider to use with this view object.
		The default implementation returns the parent view's Ui provider,
		or null if the view does not have a parent or the parent does not
		have a ui provider.
		*/
	virtual P<IUiProvider> determineUiProvider()
	{
		return (_pParentViewWeak != nullptr) ? _pParentViewWeak->getUiProvider() : nullptr;
	}

private:
	/** Should not be called directly. Use reinitCore() instead.	
	*/
	void _deinitCore();


	/** Should not be called directly. Use reinitCore() instead.*/
	void _initCore();


protected:
	DefaultProperty<bool>		_visible;
	DefaultProperty<UiMargin>	_margin;
	DefaultProperty<UiMargin>	_padding;
	DefaultProperty<Rect>		_bounds;

	DefaultProperty<HorizontalAlignment>	_horizontalAlignment;
	DefaultProperty<VerticalAlignment>		_verticalAlignment;

	P<IUiProvider>			_pUiProvider;
		

private:
	View*					_pParentViewWeak = nullptr;
	P<IViewCore>			_pCore;

	std::list< P<IBase> >	_propertySubs;

	DefaultProperty<SizingInfo>		_sizingInfo;
};

}

#endif

