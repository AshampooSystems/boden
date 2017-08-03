#include <bdn/init.h>
#include <bdn/View.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/debug.h>

namespace bdn
{

BDN_SAFE_STATIC_IMPL(Mutex, View::getHierarchyAndCoreMutex );

View::View()
	: _visible(true) // most views are initially visible
    , _preferredSizeHint( Size::none() )
    , _preferredSizeMinimum( Size::none() )
    , _preferredSizeMaximum( Size::none() )
{
	initProperty<bool, IViewCore, &IViewCore::setVisible, (int)PropertyInfluence_::none>(_visible);

	initProperty<UiMargin, IViewCore,  &IViewCore::setMargin, (int)PropertyInfluence_::parentPreferredSize | (int)PropertyInfluence_::parentLayout>(_margin);

	initProperty< Nullable<UiMargin>, IViewCore, &IViewCore::setPadding, (int)PropertyInfluence_::preferredSize | (int)PropertyInfluence_::childLayout>(_padding);

	initProperty<Point, IViewCore, nullptr, (int)PropertyInfluence_::none>(_position);
    initProperty<Size, IViewCore, nullptr, (int)PropertyInfluence_::childLayout>(_size);

    initProperty<HorizontalAlignment, IViewCore, &IViewCore::setHorizontalAlignment, (int)PropertyInfluence_::parentLayout>(_horizontalAlignment);
    initProperty<VerticalAlignment, IViewCore, &IViewCore::setVerticalAlignment, (int)PropertyInfluence_::parentLayout>(_verticalAlignment);

    initProperty<Size, IViewCore, &IViewCore::setPreferredSizeMinimum, (int)PropertyInfluence_::preferredSize>(_preferredSizeMinimum);
    initProperty<Size, IViewCore, &IViewCore::setPreferredSizeMaximum, (int)PropertyInfluence_::preferredSize>(_preferredSizeMaximum);
    initProperty<Size, IViewCore, &IViewCore::setPreferredSizeHint, (int)PropertyInfluence_::preferredSize>(_preferredSizeHint);
}

View::~View()
{
	// We have to be careful here to ensure that the last reference to the core object is not deleted
    // from some other thread (this destructor might be called from another thread).
    // _deinitCore takes care of this.

    _deinitCore();
}

void View::deleteThis()
{
	// this is called when the view is DEFINITELY going to be deleted.
	// Weak references have already been detached.

	// this is called when the reference count reaches zero.

	// Note that this may happen in ANY thread.

	// Because of this it is important that we detach all child views BEFORE
	// our destructor starts to execute. That is because the parentView references to us
	// are weak and
	// and must be set to null before the destruction starts. Otherwise a getParentView()
	// call in another thread might return a strong pointer to an object that has already
	// started destruction. And since it is a strong pointer the caller will assume that
	// it remains valid as long as the pointer exists - so crashes may occur.

	// To avoid that we must first detach the child views.
    removeAllChildViews();
    
	RequireNewAlloc<Base, View>::deleteThis();
}


Rect View::adjustAndSetBounds(const Rect& requestedBounds)
{
    P<IViewCore> pCore = getViewCore();

    Rect adjustedBounds;
    if(pCore!=nullptr)
        adjustedBounds = pCore->adjustAndSetBounds(requestedBounds);
    else
        adjustedBounds = requestedBounds;

    // update the position and size properties.
    // Note that the property changes will automatically cause our propertyChanged method
    // to be called, which will schedule any additional operations that should follow
    // (like re-layout when the size changes, etc.).
    _position = adjustedBounds.getPosition();
    _size = adjustedBounds.getSize();

    return adjustedBounds;
}



Rect View::adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const
{
    P<const IViewCore> pCore = getViewCore();

    if(pCore!=nullptr)
        return pCore->adjustBounds(requestedBounds, positionRoundType, sizeRoundType);
    else
        return requestedBounds;
}




void View::invalidateSizingInfo( InvalidateReason reason )
{
    if( isBeingDeletedBecauseReferenceCountReachedZero() )
	{
		// this happens when invalidateSizingInfo is called during the destructor.
		// In this case we do not schedule the invalidation, since the view
		// will be gone anyway.

		// So, do nothing.
        return;
	}

    if( Thread::isCurrentMain() )
    {
        // clear cached sizing data
        _preferredSizeManager.clear();

        // pass the operation to the core. The core will take care
        // of invalidating the layout, if necessary
        P<IViewCore> pCore = getViewCore();
        if(pCore!=nullptr)
            pCore->invalidateSizingInfo(reason);

        P<View> pParentView = getParentView();
        if(pParentView!=nullptr)
            pParentView->childSizingInfoInvalidated( this );
    }
    else
    {
		// schedule the invalidation to be done from the main thread.
		P<View> pThis = this;

		asyncCallFromMainThread(
				[pThis, reason]()
				{
					pThis->invalidateSizingInfo(reason);
				}
		);
    }
}


void View::needLayout( InvalidateReason reason )
{
    if( isBeingDeletedBecauseReferenceCountReachedZero() )
	{
		// this happens when invalidateSizingInfo is called during the destructor.
		// In this case we do not schedule the invalidation, since the view
		// will be gone anyway.

		// So, do nothing.
        return;
	}

    if( Thread::isCurrentMain() )
    {
        P<IViewCore> pCore = getViewCore();

        // forward the request to the core. Depending on the platform
        // it may be that the UI uses a layout coordinator provided by the system,
        // rather than our own.
        if(pCore!=nullptr)
            pCore->needLayout(reason);
    }
    else
    {
		// schedule the invalidation to be done from the main thread.
		P<View> pThis = this;

		asyncCallFromMainThread(
				[pThis, reason]()
				{
					pThis->needLayout(reason);
				});
    }
}


void View::verifyInMainThread(const String& methodName) const
{
	if(!Thread::isCurrentMain())
		programmingError(methodName + " must be called from main thread.");
}


double View::uiLengthToDips( const UiLength& length) const
{
	verifyInMainThread("View::uiLengthToDips");	

    if(length.isNone())
        return 0;

    else if(length.unit == UiLength::Unit::dip)
		return length.value;	

    else
    {
	    P<IViewCore> pCore = getViewCore();

	    if(pCore!=nullptr)
    		return pCore->uiLengthToDips(length);
        else
            return 0;
    }    
}

Margin View::uiMarginToDipMargin( const UiMargin& uiMargin) const
{
	verifyInMainThread("View::uiMarginToPixelMargin");	

	P<IViewCore> pCore = getViewCore();

	if(pCore!=nullptr)
		return pCore->uiMarginToDipMargin(uiMargin);
	else
		return Margin();	
}


void View::childSizingInfoInvalidated(View* pChild)
{
    if( isBeingDeletedBecauseReferenceCountReachedZero() )
	{
		// this happens when childSizingInfoInvalidated is called during the destructor.
		// In this case we do not schedule the invalidation, since the view
		// will be gone anyway.

		// So, do nothing.
        return;
	}

    P<IViewCore> pCore = getViewCore();

    if(pCore!=nullptr)
        pCore->childSizingInfoInvalidated(pChild);
}

void View::_setParentView(View* pParentView)
{
	MutexLock lock( getHierarchyAndCoreMutex() );

	bool callReinitCoreHere = true;

	// note that we do not have special handling for the case when the "new" parent view
	// is the same as the old parent view. That case can happen if the order position
	// of a child view inside the parent is changed. We use the same handling for that
	// as for the case of a different handling: ask the core to update accordingly.
	// And the core gets the opportunity to deny that, causing us to recreate the core
	// (maybe in some cases the core cannot change the order of existing views).

	_pParentViewWeak = pParentView;

	P<IUiProvider>	pNewUiProvider = determineUiProvider();
			
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
			if(_pCore->tryChangeParentView(_pParentViewWeak) )
				callReinitCoreHere = false;
		}
		else
		{
			// schedule the update to happen in the main thread.
			// Do not reinit the core from THIS thread.
			callReinitCoreHere = false;

			P<IViewCore>	pCore = _pCore;
			P<View>			pThis = this;
			P<View>			pParentView = _pParentViewWeak;	// strong reference - we need to keep this alive during the call
			asyncCallFromMainThread(
				[pThis, pCore, pParentView]()
				{
					MutexLock( pThis->getHierarchyAndCoreMutex() );

					if(pThis->_pCore == pCore && pThis->_pParentViewWeak==pParentView)
					{
						if(! pThis->_pCore->tryChangeParentView(pThis->_pParentViewWeak) )
							pThis->reinitCore();
					}
			} );
		}
	}

	if(callReinitCoreHere)
		reinitCore();

	// note that there is no need to update the UI provider of the child views.
	// If our UI provider changed then we will reinit our core. That automatically
	// causes our child cores to be reinitialized. Which in turn updates their view provider.
}


	
void View::reinitCore()
{
    MutexLock		lock( getHierarchyAndCoreMutex() );

	_deinitCore();

	// at this point our core and the core of our child views is null.
	// The referenced core objects might still exist for a few moments (pending
	// to be destroyed from the main thread), but they are not connected to us
	// anymore.

	// now schedule a new core to be created from the main thread. Keep ourselves alive while we do that.
	_initCore();
}


void View::_deinitCore()
{
    MutexLock		lock( getHierarchyAndCoreMutex() );
		
	std::list< P<View> > childViewsCopy;
	getChildViews( childViewsCopy );

    P<IViewCore>	pCoreToReleaseFromMainThread;
    
    if(_pCore!=nullptr && !Thread::isCurrentMain())
    {
        // we cannot release our reference to the old core from here.
        // Cores must ONLY be deleted from the main thread.
        // So we schedule it to be released later.
        pCoreToReleaseFromMainThread = _pCore;

        // note that it is not harmful for the old core to still exist
        // a few moments longer. When a new core is to be created then
        // that new core is significantly different (other UI provider or
        // other parent), so it will not "overlap" with the old core.
        // And if the core is deinitialized because this object is about
        // to be deleted then that is not harmful either because the
        // Cores use WeakP to access the outer view. So it will safely
        // detect if this outer view object is already gone.
    }
        
    _pCore = nullptr;
    	
	// also release the core of all child views
	for( auto pChildView: childViewsCopy )
		pChildView->_deinitCore();

    
    // If we need to release the old core from the main thread then we schedule
    // that here now.
    if(pCoreToReleaseFromMainThread!=nullptr)
	{
		// schedule the core to be released from the main thread.
		// IMPORTANT: there is a possible race condition here: if the main thread
		// executes the lambda before this function here finishes, then WE have the
		// last reference and will destruct the object from this thread.
		// To avoid that we have to ensure that our reference is released before
		// the mainthread call is scheduled.

		IViewCore* pCore = pCoreToReleaseFromMainThread.detachPtr();

		asyncCallFromMainThread(
				[pCore]()
				{
					pCore->releaseRef();
				}
		);
	}
}


void View::_initCore()
{
	// initCore might throw an exception in some cases (for example if the view type
	// is not supported).
	// If it is called from the main thread then we want to propagate that exception upwards.
	// If it is not called from the main thread then we simply can't propagate exceptions,
	// since the execution happens asynchronously. So in that case we silently ignore it.

	if( ! Thread::isCurrentMain() )
	{
		// keep ourselves alive during this call
		P<View> pThis = this;

		asyncCallFromMainThread( [pThis]()
								{
									pThis->_initCore();
								} );
	}
	else
	{
		// note that there is no need to protect against race conditions here,
		// even though the actual init might be done asynchronously at a later time
		// from another thread.

		// Cores are ONLY created in the main thread.
		// So if another worker thread calls _initCore then that change will also
		// be scheduled and executed AFTER the our scheduled init. So the order is ok.

		// The only tricky thing here is if _initCore is called from the main thread.
		// In that case the call is NOT scheduled and will execute immediately.
		// So it could potentially execute before our scheduled init and ordering
		// would be incorrect.

		// BUT it does not matter wether ordering is correct. The first init to executed
		// after a deinit will do the work with up-to-date parameters. And any pending
		// scheduled init will then see that the core is already there and simply do nothing.

		MutexLock		lock( getHierarchyAndCoreMutex() );

		// If the core is not null then someone else called init while we were
		// scheduled. That is ok, since the core creation always uses up-to-date
		// parameters. So everything is already done the way we would have done it.
		// So only proceed if the core is null.
		if(_pCore==nullptr)
		{			
			_pUiProvider = determineUiProvider();

			if(_pUiProvider!=nullptr)
				_pCore = _pUiProvider->createViewCore( getCoreTypeName(), this);

			std::list< P<View> > childViewsCopy;
			getChildViews( childViewsCopy );			

			for(auto pChildView: childViewsCopy)
				pChildView->_initCore();

			// our old sizing info is obsolete when the core has changed.
			invalidateSizingInfo( View::InvalidateReason::standardPropertyChanged );
		}		
	}
}



Size View::calcPreferredSize( const Size& availableSpace ) const
{
	verifyInMainThread("View::calcPreferredSize");
	
	Size preferredSize;
	if( ! _preferredSizeManager.get(availableSpace, preferredSize) )
	{
		P<IViewCore> pCore = getViewCore();

		if(pCore!=nullptr)
		{
			preferredSize = pCore->calcPreferredSize( availableSpace );
			_preferredSizeManager.set( availableSpace, preferredSize );
		}
	}

	return preferredSize;
}





}

