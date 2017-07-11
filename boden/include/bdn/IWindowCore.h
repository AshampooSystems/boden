#ifndef BDN_IWindowCore_H_
#define BDN_IWindowCore_H_

#include <bdn/IViewCore.h>

namespace bdn
{


/** The core for a top level window.*/
class IWindowCore : BDN_IMPLEMENTS IViewCore
{
public:


    
	/** Tells the window to auto-size itself. The window size will be adapted according
		to the preferred size of the content view. The window also takes other considerations
		into account, like the size of the screen it is on.
		
		Note that the update happens asynchronously, so the window will usually not have the
		new size yet when the function returns.

        Usually the auto-sizing is done by the same layout system that also
        processes IViewCore::needLayout() requests.
        
        The auto sizing should happen at a point in time when all pending sizing info updates
        have already been processed.
		*/
	virtual void requestAutoSize()=0;


	/** Tells the window to center itself on the screen.

		The update will happen asynchronously, so the position might not have been updated yet
		when the function returns.


        Usually the centering is done by the same layout system that also
        processes IViewCore::needLayout() requests.
        
        The centering should happen at a point in time when all pending sizing info updates
        and auto sizing requests have already been processed.
	*/
	virtual void requestCenter()=0;


	/** Changes the window's title.*/
	virtual void setTitle(const String& title)=0;


	/** Returns the area where the Window's content window
		should be.*/
	virtual Rect getContentArea()=0;


	/** Calculates the size that the whole window from the size of the content area.
		This usually adds sizes for the window border (if it has any), title bar, menu bar
		and the like.

        If the content viewport has constraints for the current display that make adjustments
        to the specified contentSize necessary then the function will round up the contentSize
        to a representable size and then return the corresponding window size.
	*/
	virtual Size calcWindowSizeFromContentAreaSize(const Size& contentSize)=0;


	/** Calculates the size of the content area from the size of the whole window.
	
		This is the inverse of calcWindowSizeFromContentAreaSize().	
        
        The returned size should never be negative. If the specified window size
        is smaller than the size needed for the window frame, titlebar, etc and a
        size with a negative component would be the result of the calculation then
        that component should be set to zero.
	*/
	virtual Size calcContentAreaSizeFromWindowSize(const Size& windowSize)=0;


	/** Returns the minimum size of the window, so that all the essential window controls
		are visible (close, maximize buttons, etc.).
        
        The function should NOT include the size constraints from the outer window object in its
        calculation (i.e. View::preferredSizeMinimum() should be ignored );
        */
	virtual Size getMinimumSize() const=0;
    

	/** Returns the area of the screen that can be used by windows.
		That excludes taskbars, sidebars and the like (if they are always visible).
		The returned rect applies only to the screen that the window is currently on.
		Other screens can have different window areas.
        
        Note that the work area position may have negative coordinates on systems
        with multiple monitors. That can be normal.
     
        */
	virtual Rect getScreenWorkArea() const=0;


    
	
};


}

#endif
