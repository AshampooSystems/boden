#include <bdn/init.h>
#include <bdn/View.h>

#include <bdn/LayoutCoordinator.h>

namespace bdn
{

View::View()
{
	initProperty<bool, IViewCore, &IViewCore::setVisible>(_visible);
	initProperty<UiMargin, IViewCore, &IViewCore::setMargin>(_margin);
	initProperty<UiMargin, IViewCore, &IViewCore::setPadding>(_padding);
	initProperty<Rect, IViewCore, &IViewCore::setBounds>(_bounds);
}

View::~View()
{
	// We have to manually deinit the core here (if we have one) to ensure that it is not deleted
	// from a thread other than the main thread.
	_deinitCore();
}

void View::needSizingInfoUpdate()
{
	LayoutCoordinator::get()->viewNeedsSizingInfoUpdate(this);
}


void View::needLayout()
{
	LayoutCoordinator::get()->viewNeedsLayout(this);
}


void View::verifyInMainThread(const String& methodName) const
{
	if(!Thread::isCurrentMain())
		throw ProgrammingError(methodName + " must be called from main thread.");
}

Margin View::uiMarginToPixelMargin( const UiMargin& uiMargin) const
{
	verifyInMainThread("View::uiMarginToPixelMargin");	

	P<IViewCore> pCore = getViewCore();

	if(pCore!=nullptr)
		return pCore->uiMarginToPixelMargin(uiMargin);
	else
		return Margin();	
}

void View::updateSizingInfo()
{
	SizingInfo info;

	info.preferredSize = calcPreferredSize();

	if(info!=_sizingInfo)
	{
		_sizingInfo = info;
		
		P<View> pParentView = getParentView();

		if(pParentView!=nullptr)
		{
			// our parent needs to update its own sizing
			pParentView->needSizingInfoUpdate();

			// AND, since our sizing info has changed the parent also needs
			// to re-layout us and our siblings
			pParentView->needLayout();
		}
	}
}


void View::_setParentView(View* pParentView)
{
	MutexLock lock( getHierarchyAndCoreMutex() );

	bool mustReinitCore = true;

	// note that we do not have special handling for the case when the "new" parent view
	// is the same as the old parent view. That case can happen if the order position
	// of a child view inside the parent is changed. We use the same handling for that
	// as for the case of a different handling: ask the core to update accordingly.
	// And the core gets the opportunity to deny that, causing us to recreate the core
	// (maybe in some cases the core cannot change the order of existing views).

	_pParentViewWeak = pParentView;

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
					MutexLock( pThis->getHierarchyAndCoreMutex() );

					if(pThis->_pCore == pCore && pThis->_pParentViewWeak==pParentView)
					{
						if(! pThis->_pCore->tryChangeParentView(pThis->_pParentViewWeak) )
							pThis->reinitCore();
					}
			} );
		}
	}

	if(mustReinitCore)
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
	// we must not release the core from another thread. So we do that from the
	// main thread.

	MutexLock		lock( getHierarchyAndCoreMutex() );
		
	P<IViewCore>	pOldCore = _pCore;

	_pCore = nullptr;

	std::list< P<View> > childViewsCopy;
	getChildViews( childViewsCopy );
	
	// also release the core of all child views
	for( auto pChildView: childViewsCopy )
		pChildView->_deinitCore();

	// now schedule the core reference to be released from the main thread.
	// note that we do nothing in the scheduled function. We only use this to keep the core alive
	// and cause its final release to be called from the main thread.
	callFromMainThread( [pOldCore](){} );
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
			needSizingInfoUpdate();
		}		
	}
}



Size View::calcPreferredSize() const
{
	verifyInMainThread("View::calcPreferredSize");

	P<IViewCore> pCore = getViewCore();

	if(pCore!=nullptr)
		return pCore->calcPreferredSize();
	else
		return Size(0, 0);
}

	
int View::calcPreferredHeightForWidth(int width) const
{
	verifyInMainThread("View::calcPreferredHeightForWidth");

	P<IViewCore> pCore = getViewCore();

	if(pCore!=nullptr)
		return pCore->calcPreferredHeightForWidth(width);
	else
		return 0;
}


int View::calcPreferredWidthForHeight(int height) const
{
	verifyInMainThread("View::calcPreferredWidthForHeight");

	P<IViewCore> pCore = getViewCore();

	if(pCore!=nullptr)
		return pCore->calcPreferredWidthForHeight(height);
	else
		return 0;
}


}

