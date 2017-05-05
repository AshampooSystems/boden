#ifndef BDN_IScrollViewCore_H_
#define BDN_IWindowCoreBDN_IScrollViewCore_H__H_

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
    

    /** Sizes and positions the content view.*/
    virtual void layout()=0;
	
};


}

#endif
