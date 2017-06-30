#include <bdn/init.h>
#include <bdn/LayoutCoordinator.h>

#include <bdn/NotImplementedError.h>
#include <bdn/log.h>

#include <vector>

namespace bdn
{

LayoutCoordinator::LayoutCoordinator()
{
}


void LayoutCoordinator::windowNeedsAutoSizing(Window* pWindow)
{
	MutexLock lock( _mutex );

	_windowAutoSizeSet.insert( pWindow );

	needUpdate();
}


void LayoutCoordinator::windowNeedsCentering(Window* pWindow)
{
	MutexLock lock( _mutex );

	_windowCenterSet.insert( pWindow );

	needUpdate();
}


void LayoutCoordinator::viewNeedsSizingInfoUpdate(View* pView)
{
	MutexLock lock( _mutex );

	_sizingInfoSet.insert( pView );

	needUpdate();
}

void LayoutCoordinator::viewNeedsLayout(View* pView)
{
	MutexLock lock( _mutex );

	_layoutSet.insert( pView );

	needUpdate();
}



void LayoutCoordinator::needUpdate()
{
	if(!_updateScheduled)
	{
		P<LayoutCoordinator> pThis = this;

		_updateScheduled = true;

		// note that we use asyncCallFromMainThread here. I.e. if we are
		// in the main thread then this is still scheduled for later, rather
		// than run immediately.
		// That is what we want, because that allows us to collect and combine
		// multiple operations.
		asyncCallFromMainThread(
			[pThis]()
			{
				{
					MutexLock lock( pThis->_mutex );
					pThis->_updateScheduled = false;
				}

				pThis->mainThreadUpdateNow();
			} );
	}
}


	


void LayoutCoordinator::mainThreadUpdateNow()
{
	if(_inUpdateNow)
	{
		// this means that one of the views has caused events to be handled during
		// its sizing, layout or other call.
		// Ignore this here.
		return;
	}
	
	_inUpdateNow = true;

	try
	{
		// first we have to determine the order in which we should
		// update the views.
		// For resizing the optimal order is child-to-parent. Because if
		// a child's child changes then this might influence the parent's size.
		// If we would update the parent first and then the child, then the child update
		// might cause the parent to need another update afterwards.

		struct ToDo
		{
			ToDo()
				: level(-1)
			{
			}

			ToDo(const P<View>& pView)
			{
				this->pView = pView;

				// find out the view's level inside the UI tree
				this->level = 0;
				P<View> pCurrParent = pView->getParentView();
				while(pCurrParent!=nullptr)
				{
					this->level++;
					pCurrParent = pCurrParent->getParentView();
				}
			}

			bool operator<(const ToDo& o) const
			{
				return (level<o.level || (level==o.level && pView.getPtr() < o.pView.getPtr() ) );
			}

			P<View> pView;
			int		level;
		};


		bool anySizingInfosUpdated = false;

		// note that this loop is structured in a way so that new objects can be added during
		// the sizing info update of each view.
		{
			std::list< ToDo > toDoList;
			while(true)
			{
				std::set< P<View> > newlyAddedSet;

				{
					MutexLock lock( _mutex );
					newlyAddedSet = _sizingInfoSet;
					_sizingInfoSet.clear();
				}
				
				// add the new requests to the todo list.
				// Keep the global UI mutex locked during this, because we do not
				// want any changes made to the view hierarchy during this (since we compute
				// our level value based on the hierarchy)
				if(!newlyAddedSet.empty())
				{
					{
						MutexLock lock( View::getHierarchyAndCoreMutex() );

						toDoList.insert( toDoList.end(), newlyAddedSet.begin(), newlyAddedSet.end() );
					}

					// re-sort the list. We want inverted order: higher levels (=children) first.
					// Note that we accept that if any changes are made to the UI hierarchy during this
					// then we will have an unoptimal order. But that case should be very rare, 
					// and the end result will still be correct. So we ignore it.
					std::vector<ToDo> temp(toDoList.begin(), toDoList.end());

					std::sort(	temp.begin(),
								temp.end(),
								[](const ToDo& a, const ToDo& b)
								{
									// invert the order. We want higher levels (=children) first.
									// So we return the result of b<a instead of a<b.
									return b<a;
								} );

					toDoList.clear();
					toDoList.insert( toDoList.end(), temp.begin(), temp.end() );

                    // remove duplicates from the toDoList. We can get those if the same view
                    // is added in subsequent iterations after we moved the initial entry from
                    // the set to the toDoList.
                    P<View> pPrevView;
                    for(auto it = toDoList.begin(); it!=toDoList.end(); )
                    {
                        if(it->pView == pPrevView)
                            it = toDoList.erase(it);
                        else
                        {
                            pPrevView = it->pView;
                            ++it;
                        }
                    }
				}		

				if(toDoList.empty())
				{
					// done.
					break;
				}

				ToDo toDo = toDoList.front();
				toDoList.pop_front();

				anySizingInfosUpdated = true;
				
                try
                {
				    toDo.pView->updateSizingInfo();		
                }
                catch(std::exception& e)
                {
                    handleException(&e, "View::updateSizingInfo");
                }
                catch(...)
                {
                    handleException(nullptr, "View::updateSizingInfo");                    
                }		
			}
		}

		// the sizing info update may have changed one or more properties of the views.
		// Their change notifications will now be in the event queue. Since those might trigger
		// more sizing updates (for example, of parent views) we want to make sure that
		// we execute all of those before we continue with the dependent actions.
		// So if any sizing info was updated we end here and schedule another update.
		// Since that has the same priority as the property notifications this ensures
		// that they have all been handled and any subsequent update requests have been added.
		if(anySizingInfosUpdated)			
			needUpdate();
		else
		{
			// no sizing infos updated. Continue with other tasks.

			// now we do window auto sizing
			bool anyWindowsAutoSized = false;
			{		
				// note that the order in which we auto-size the windows
				// does not matter, since all windows are top-level

				std::set< P<Window> > toDoSet;
				while(true)
				{
					{
						MutexLock lock( _mutex );
						toDoSet.insert( _windowAutoSizeSet.begin(), _windowAutoSizeSet.end() );
						_windowAutoSizeSet.clear();
					}				

					if(toDoSet.empty())
					{
						// done.
						break;
					}
					
					P<Window> pWindow = *toDoSet.begin();
					toDoSet.erase(toDoSet.begin());

					anyWindowsAutoSized = true;

					try
					{
						pWindow->autoSize();
					}
					catch(std::exception& e)
					{
						handleException(&e, "Window::autoSize");
					}
					catch(...)
					{
						handleException(nullptr, "Window::autoSize::layout");                    
					}		
				}	
			}		

			// note that for window sizing we also need to do the same async interruption here that
			// we do after sizing info updates. While window sizes cannot impact parents, they CAN cause
			// layout operations of inner windows. And we do want all of those to be in
			// our queues, so that we can remove duplicates.
			// So again, we reschedule another update if we have done anything.
			if(anyWindowsAutoSized)
				needUpdate();
			else
			{	
				bool anyLayoutDone = false;

				// now do the layout operations.
				// Note that layout operations that have been triggered by any
				// of the sizing info updates, or window auto-sizing are included in this!
				{
					ToDo nextToDo;

					{
						MutexLock lock( _mutex );

						// note that we only remove one view at a time from the pending set.
						// The reason is that we need to handle pending events after each
						// layout.
						// So we select the one we want (parent first order) and leave the
						// others in the set for now.

						for(auto& pView: _layoutSet)
						{
							// construct ToDo object so that we know the level.
							ToDo toDo(pView);

							if(nextToDo.pView==nullptr || toDo<nextToDo)
								nextToDo = toDo;
						}

						if(nextToDo.pView!=nullptr)
						{
							// remove the view we selected from the set
							_layoutSet.erase( nextToDo.pView );
						}
					}
				

					if(nextToDo.pView!=nullptr)
					{	
						anyLayoutDone = true;

						try
						{
							P<IViewCore> pCore = nextToDo.pView->getViewCore();
							if(pCore!=nullptr)
								pCore->layout();
						}
						catch(std::exception& e)
						{
							handleException(&e, "IViewCore::layout");                    
						}
						catch(...)
						{
							handleException(nullptr, "IViewCore::layout");                    
						}									
					}
				}

				// if a parent view has been layouted then we want events to be handled at this point,
				// before we continue with our todo list (which is in parent-first order).
				// The reason is that child sizes may have changed and the property change notifications for
				// those changes are queued up in the dispatcher queue now (if child sizes changed). And those
				// queued up change notifications will likely trigger a layout request for the child
				// view they belong to. And since the child view might already be in our todo list it is important
				// that we wait for the queued up events to be handled before we proceed with the layout.
				// Only if we wait can we detect the duplicate layout and prevent the child from being
				// layouted twice.
				// So if we did layout anything then we schedule another update. Otherwise we continue.
				if(anyLayoutDone)
					needUpdate();
				else
				{
					// now we do window centering
					{		
						// note that the order in which we auto-size the windows
						// does not matter, since all windows are top-level

						std::set< P<Window> > toDoSet;
						while(true)
						{
							{
								MutexLock lock( _mutex );
								toDoSet.insert( _windowCenterSet.begin(), _windowCenterSet.end() );
								_windowCenterSet.clear();
							}				

							if(toDoSet.empty())
							{
								// done.
								break;
							}
					
							P<Window> pWindow = *toDoSet.begin();
							toDoSet.erase(toDoSet.begin());

							try
							{
								pWindow->center();
							}
							catch(std::exception& e)
							{
								handleException(&e, "Window::center");
							}
							catch(...)
							{
								handleException(nullptr, "Window::center");                    
							}		
						}	
					}		
				}
			}
		}
	}
	catch(...)
	{
		_inUpdateNow = false;
		throw;
	}

	_inUpdateNow = false;
}


void LayoutCoordinator::handleException(const std::exception* pExceptionIfAvailable, const String& functionName)
{
    // log and ignore
    if(pExceptionIfAvailable!=nullptr)
        logError(*pExceptionIfAvailable, "Exception in "+functionName+" during LayoutCoordinator updating. Ignording.");
    else
        logError("Exception pf unknown type in "+functionName+" during LayoutCoordinator updating. Ignording.");

}

}
