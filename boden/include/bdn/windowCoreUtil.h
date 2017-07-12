#ifndef BDN_windowCoreUtil_H_
#define BDN_windowCoreUtil_H_

#include <bdn/Window.h>

namespace bdn
{



/** A default implementation for implementing the layout of a Window object (see IViewCore::layout() ),
    intended for use by the window's core object.
    This function can be called by the core if no special layout procedure is needed and the
    only task to perform is to arrange the content view.

    The specified contentArea must be the area inside the window that is used to display the contents.

    The default implementation will arrange the content view inside this area, according to the
    applicable margins and padding.
    */
void defaultWindowLayoutImpl(Window* pWindow, const Rect& contentArea);
	
    
/** Default implementation for auto-sizing the window (see requestAutoSize()), intended for use by the window's core object.
    This function can be called by the core if no special auto sizing procedure is needed and the
    only task to perform is to size the window according to its preferred size and the available screen space.
        
    \param screenWorkAreaSize the size of the screen work area (the part of the screen that can be used
        to display windows). This excludes taskbars, docks, etc.
    */
void defaultWindowAutoSizeImpl(Window* pWindow, const Size& screenWorkAreaSize );


/** Default implementation for centering the window (see requestCenter()), intended for use by the window's core object.
    This function can be called by the core if no special centering procedure is needed and the
    only task to perform is to position the window on the screen.
        
    \param screenWorkArea the screen work area (the part of the screen that can be used
        to display windows). This excludes taskbars, docks, etc.
    */
void defaultWindowCenterImpl(Window* pWindow, const Rect& screenWorkArea );
	


}

#endif

