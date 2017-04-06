#ifndef BDN_LayoutCoordinator_H_
#define BDN_LayoutCoordinator_H_


#include <bdn/Window.h>
#include <bdn/View.h>

#include <set>

namespace bdn
{
	

/** Coordinates the updating of the layout for user interface components.

	Whenever an event happens that might require a re-layout, the corresponding component
	notifies the layout coordinator.

	The coordinator collects these requests and tries to batch together
	multiple consecutive changes that modify the layout into a single update operation.	
	Note that the layout update will still happen almost immediately.
	
	The coordinator also optimizes the order of multiple update operations,
	to ensure that no duplicate work is done.

	Usually the global coordinator object should be used (see LayoutCoordinator::get()).

	This class is thread-safe.
*/
class LayoutCoordinator : public Base
{
public:
	LayoutCoordinator();

	/** Registers a view for a size information update. This should be called when sizing
		parameters (like padding, etc) or the view contents change and the preferred/minimum/maximum
		sizes of the view may have changed.

		For view containers this should also be called when child views change in
		a way that could influence the container's preferred/minimum/maximum size.
	*/
	void viewNeedsSizingInfoUpdate(View* pView);


	/** Registers a view for re-layout. This should be called when any of the child
		views have changed their size or any of the parameters that affect their layout
		(like margins, alignment, etc.)
		*/
	void viewNeedsLayout(View* pView);



	/** Registers a top level window for auto-sizing.*/
	void windowNeedsAutoSizing(Window* pWindow);

	/** Registers a top-level window for centering on the screen.*/
	void windowNeedsCentering(Window* pWindow);



	/** Returns the global coordinator object.*/
	static P<LayoutCoordinator> get()
	{
		static SafeInit<LayoutCoordinator> init;

		return init.get();
	}
	
protected:
	void needUpdate();

	void mainThreadUpdateNow();

    /** This function is called when an exception occurs during while a
        view is being updated. The default implementation logs the exception
        and does nothing otherwise.
        
        \param pExceptionIfAvailable is non-null if the exception is a std::exception object.
        It is null if it is another kind of exception.

        \param functionName is the name of the function that produced the exception.

        */
    virtual void handleException(const std::exception* pExceptionIfAvailable, const String& functionName);

	Mutex				_mutex;
	
	std::set< P<View> > _sizingInfoSet;
	std::set< P<View> > _layoutSet;

	std::set< P<Window> > _windowAutoSizeSet;
	std::set< P<Window> > _windowCenterSet;

	bool _updateScheduled = false;

	bool _inUpdateNow = false;
};


}


#endif

