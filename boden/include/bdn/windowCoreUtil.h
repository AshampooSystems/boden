#ifndef BDN_windowCoreUtil_H_
#define BDN_windowCoreUtil_H_

#include <bdn/Window.h>

namespace bdn
{

    
/** A default implementation for the IViewCore::calcPreferredSize method for a window.

    \param pWindow the window for which the preferred size is to be calculated
    \param availableSpace the available space for the whole window (including its border)
    \param border the size of the Window's non-client border (including the title bar). There are platforms where
        windows to platforms where windows do not have a border or a title bar - on those platforms border should
        simply be a zero Margin object.
    \param minWindowSize the minimum size a window can have on the platform (including the border and title bar).
        This can be Size(0,0) if the platform does not impose a minimum size limit.
    */
Size defaultWindowCalcPreferredSizeImpl(Window* pWindow, const Size& availableSpace, const Margin& border, const Size& minWindowSize );


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

