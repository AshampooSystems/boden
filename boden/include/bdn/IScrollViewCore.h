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

         The function also supports the special "infinity" floating point value.
         If positive infinity is specified as the rect position then the function will scroll
         to the end of the client area in that direction.        
        */
    virtual void scrollClientRectToVisible(const Rect& clientRect)=0;
	
};


}

#endif
