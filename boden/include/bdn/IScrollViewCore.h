#ifndef BDN_IScrollViewCore_H_
#define BDN_IScrollViewCore_H_

#include <bdn/IViewCore.h>

namespace bdn
{


/** The core for scroll views.*/
class IScrollViewCore : BDN_IMPLEMENTS IViewCore
{
public:

	/** Controls wether or not the view scrolls horizontally.*/
	virtual void setHorizontalScrollingEnabled(const bool& enabled)=0;

    /** Controls wether or not the view scrolls vertically.*/
	virtual void setVerticalScrollingEnabled(const bool& enabled)=0;
    

    /** Changes the scroll position so that the specified part of the
        client area is visible.

        The specified rect is in client coordinates (see \ref layout_box_model.md).

        It is ok to specify coordinates outside the client area. If the rect exceeds
        the client area then the function will make sure that the edge of the client area
        in that direction is visible.

        If the specified target rect is bigger than the viewport then of course not all
        of the rect can be visible. In this case the scroll view will scroll the minimal amount
        possible, to make as much of the target rect visible as possible. For example, if the scroll position
        was previously to the right of the target rect then it will scroll so that the right part of the target
        rect is visible. The left part of the target rect will be out of view, since it does not fit
        into the viewport. If the scroll position before the call had been to the left of the target rect
        then after the call the left part of the target rect would be visible and the right part
        would be out of view.

        scrollClientRectToVisible also supports the special "infinity" floating point values.
        If positive infinity is specified as the rect position then the function will scroll
        to the end of the client area in that direction.
        A negative infinity position scrolls to the start, although you can of course also
        simply specify 0 instead.
        */
    virtual void scrollClientRectToVisible(const Rect& clientRect)=0;
	
};


}

#endif
