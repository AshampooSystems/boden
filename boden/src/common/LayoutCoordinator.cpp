#include <bdn/init.h>
#include <bdn/LayoutCoordinator.h>

#include <bdn/NotImplementedError.h>

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
				}		

				if(toDoList.empty())
				{
					// done.
					break;
				}

				ToDo toDo = toDoList.front();
				toDoList.pop_front();

				toDo.pView->updateSizingInfo();		
			}
		}



		// now we do window auto sizing
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

				pWindow->autoSize();
			}	
		}		
	
		// now do the layout operations.
		// Note that layout operations that have been triggered by any
		// of the sizing info updates, or window auto-sizing are included in this!
		{
			std::list< ToDo > toDoList;
			while(true)
			{
				std::set< P<View> > newlyAddedSet;

				{
					MutexLock lock( _mutex );
					newlyAddedSet = _layoutSet;
					_layoutSet.clear();
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

					// re-sort the list
					// use the normal order: smaller levels (=parents) first.
					// Note that we accept that if any changes are made to the UI hierarchy during this
					// then we will have an unoptimal order. But that case should be very rare, 
					// and the end result will still be correct. So we ignore it.
					std::vector<ToDo> temp(toDoList.begin(), toDoList.end());

					std::sort(	temp.begin(), temp.end() );

					toDoList.clear();
					toDoList.insert( toDoList.end(), temp.begin(), temp.end() );
				}


				if(toDoList.empty())
				{
					// done.
					break;
				}	

		
				ToDo toDo = toDoList.front();
				toDoList.pop_front();

				toDo.pView->layout();
			}	
		}



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

				pWindow->center();
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

}
