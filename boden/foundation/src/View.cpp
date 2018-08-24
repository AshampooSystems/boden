#include <bdn/init.h>
#include <bdn/View.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/debug.h>


namespace bdn
{

View::View()
{
    setVisible(true); // most views are initially visible
    setPreferredSizeHint( Size::none() );
    setPreferredSizeMinimum( Size::none() );
    setPreferredSizeMaximum( Size::none() );
}

View::~View()
{
    // the core should already be deinitialized at this point (since deleteThis should have run).
    // But we call it again anyway for safety.
    _deinitCore();
}

void View::deleteThis()
{       
    // Note that deleteThis can be called from ANY thread (we allow view object references to be
    // passed to other threads, even though they are not allowed to access them directly
    // from the other thread).

    // Also note that when this is called, weak pointers to this view already consider the View to be
    // gone. I.e. WeakP::toStrong already returns null, so no new strong references can be created.
    
    // For views we have the special case that we allow their last reference to be released from any
    // thread, even though the view itself can only be used from the main thread. That allows view
    // objects to be passed to background threads without having to think about who releases the last reference.

    // When a view is destructed then we need to do a lot of things: we need to remove the child views
    // and ensure that their parent pointer is null. We also need to remove / reinit their core.
    // Those are all operations that can only be done from the main thread. So we do not want the
    // destruction to begin in any other thread.
    if( ! Thread::isCurrentMain() )
    {
        // we are not in the main thread. So we need to stop here and continue the destruction in the main thread.
        
        // note that the lambda function we post here may be executed
        // in the main thread immediately. I.e. even before we exit
        // deleteThis. However, that is OK, since we do not
        // access our object after the asyncCallFromMainThread call. The caller of deleteThis
        // also does not access the object, since deleteThis has the task of deleting
        // the object.

        // Also note that we do not need to increase the reference count when we pass "this"
        // to the main thread, since the object is already on track for deletion and no one else
        // can have or obtain a strong reference to it.
        
        asyncCallFromMainThread(
            [this]()
            {   
                deleteThis();
            } );

        return;
    }

    // We want to detach our child views before we start to destruct ourselves.
    // That ensures that the parent is still fully intact at the time when the child views
    // are destroyed. While the child views cannot access the parent itself anymore at this point
    // (since their weak parent pointers already return null), the child views might still interact
    // with the parent's sub-objects in some way. For example, they might have references to
    // the parent's properties or event notifiers. Note that event notifier references often exist implicitly
    // when an event has been posted but not yet executed.
    // So, to avoid any weird interactions we make sure that the children are fully detached before
    // we start destruction (and before our member objects start to destruct).

    removeAllChildViews();

    // also deinit our core before we start destruction (since the core might still try to access us).
    _deinitCore();

    RequireNewAlloc<Base, View>::deleteThis();
}


Rect View::adjustAndSetBounds(const Rect& requestedBounds)
{
    P<IViewCore> pCore = getViewCore();

    Rect adjustedBounds;
    if(pCore!=nullptr)
    {
        adjustedBounds = pCore->adjustAndSetBounds(requestedBounds);
        
        if( !std::isfinite(adjustedBounds.width)
           || !std::isfinite(adjustedBounds.height)
           || !std::isfinite(adjustedBounds.x)
           || !std::isfinite(adjustedBounds.y) )
        {
            // the preferred size MUST be finite.
            IViewCore* pCorePtr = pCore;
            programmingError( String( typeid(*pCorePtr).name() )+".adjustAndSetBounds returned a non-finite value: "+std::to_string(adjustedBounds.x)+" , "+std::to_string(adjustedBounds.y)+" "+std::to_string(adjustedBounds.width)+" x "+std::to_string(adjustedBounds.height));
        }
    }
    else
        adjustedBounds = requestedBounds;
    
    // update the position and size properties.
    // Note that the property changes will automatically cause our "modification influence" methods
    // to be called, which will schedule any additional operations that should follow
    // (like re-layout when the size changes, etc.).
    _setPosition( adjustedBounds.getPosition() );
    _setSize( adjustedBounds.getSize() );

    return adjustedBounds;
}



Rect View::adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const
{
    P<const IViewCore> pCore = getViewCore();

    if(pCore!=nullptr)
    {
        Rect adjustedBounds = pCore->adjustBounds(requestedBounds, positionRoundType, sizeRoundType);
        
        if( !std::isfinite(adjustedBounds.width)
           || !std::isfinite(adjustedBounds.height)
           || !std::isfinite(adjustedBounds.x)
           || !std::isfinite(adjustedBounds.y) )
        {
            // the adjusted bounds MUST be finite.
            const IViewCore* pCorePtr = pCore;
            programmingError( String( typeid(*pCorePtr).name() )+".adjustBounds returned a non-finite value: "+std::to_string(adjustedBounds.x)+" , "+std::to_string(adjustedBounds.y)+" "+std::to_string(adjustedBounds.width)+" x "+std::to_string(adjustedBounds.height));
        }
        
        return adjustedBounds;
    }
    else
        return requestedBounds;
}




void View::invalidateSizingInfo( InvalidateReason reason )
{
	Thread::assertInMainThread();

    if( isBeingDeletedBecauseReferenceCountReachedZero() )
	{
		// this happens when invalidateSizingInfo is called during the destructor.
		// In this case we do not schedule the invalidation, since the view
		// will be gone anyway.

		// So, do nothing.
        return;
	}

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


void View::needLayout( InvalidateReason reason )
{
	Thread::assertInMainThread();

    if( isBeingDeletedBecauseReferenceCountReachedZero() )
	{
		// this happens when invalidateSizingInfo is called during the destructor.
		// In this case we do not schedule the invalidation, since the view
		// will be gone anyway.

		// So, do nothing.
        return;
	}

    P<IViewCore> pCore = getViewCore();

    // forward the request to the core. Depending on the platform
    // it may be that the UI uses a layout coordinator provided by the system,
    // rather than our own.
    if(pCore!=nullptr)
        pCore->needLayout(reason);
}





double View::uiLengthToDips( const UiLength& length) const
{
	Thread::assertInMainThread();

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
	Thread::assertInMainThread();

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
	Thread::assertInMainThread();

	bool callReinitCoreHere = true;

	// note that we do not have special handling for the case when the "new" parent view
	// is the same as the old parent view. That case can happen if the order position
	// of a child view inside the parent is changed. We use the same handling for that
	// as for the case of a different handling: ask the core to update accordingly.
	// And the core gets the opportunity to deny that, causing us to recreate the core
	// (maybe in some cases the core cannot change the order of existing views).

	_parentViewWeak = pParentView;

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
		&& pParentView!=nullptr
		&& _pCore!=nullptr)
	{
		// we try to move the core to the new parent.	

		if(_pCore->tryChangeParentView(pParentView) )
			callReinitCoreHere = false;
	}

	if(callReinitCoreHere)
		reinitCore();

	// note that there is no need to update the UI provider of the child views.
	// If our UI provider changed then we will reinit our core. That automatically
	// causes our child cores to be reinitialized. Which in turn updates their view provider.
}


	
void View::reinitCore()
{
	_deinitCore();

	// at this point our core and the core of our child views is null.

	_initCore();
}


void View::_deinitCore()
{
    List< P<View> > childViewsCopy;
	getChildViews( childViewsCopy );

    _pCore = nullptr;
    	
	// also release the core of all child views
	for( auto pChildView: childViewsCopy )
		pChildView->_deinitCore();    
}


void View::_initCore()
{
	// initCore might throw an exception in some cases (for example if the view type
	// is not supported).
	// we want to propagate that exception upwards.
    
    // If the core is not null then we already have a core. We do nothing in that case.
	if(_pCore==nullptr)
	{			
		_pUiProvider = determineUiProvider();

		if(_pUiProvider!=nullptr)
			_pCore = _pUiProvider->createViewCore( getCoreTypeName(), this);

		List< P<View> > childViewsCopy;
		getChildViews( childViewsCopy );			

		for(auto pChildView: childViewsCopy)
			pChildView->_initCore();

		// our old sizing info is obsolete when the core has changed.
		invalidateSizingInfo( View::InvalidateReason::standardPropertyChanged );
	}		
}



Size View::calcPreferredSize( const Size& availableSpace ) const
{
	Thread::assertInMainThread();
	
	Size preferredSize;
	if( ! _preferredSizeManager.get(availableSpace, preferredSize) )
	{
		P<IViewCore> pCore = getViewCore();

		if(pCore!=nullptr)
		{
			preferredSize = pCore->calcPreferredSize( availableSpace );
            
            if( !std::isfinite(preferredSize.width) || !std::isfinite(preferredSize.height) )
            {
                // the preferred size MUST be finite.
                IViewCore* pCorePtr = pCore;
                programmingError( String( typeid(*pCorePtr).name() )+".calcPreferredSize returned a non-finite value: "+std::to_string(preferredSize.width)+" x "+std::to_string(preferredSize.height));
            }
            
			_preferredSizeManager.set( availableSpace, preferredSize );
		}
        else
            preferredSize = Size();
	}
    
	return preferredSize;
}





}

