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
    

    /** Changes the scroll position so that the specified area is visible.
        The specified rect coordinates refer to scroll view's inner scrolled
        coordinate system. I.e. The point (0,0) refers to the top left corner
        of the scrolled area, which is not necessarily the same
        of the top-left corner the content view (if the content view has a nonzero margin,
        or if the scroll view has nonzero padding).
        */
    virtual void scrollAreaToVisible(const Rect& area)=0;
	
};


}

#endif
