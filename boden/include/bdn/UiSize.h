#ifndef BDN_UiSize_H_
#define BDN_UiSize_H_

#include <bdn/UiLength.h>
#include <bdn/Size.h>

namespace bdn
{
	

/** Represents a size within the context of a UI view.

	In contast to #Size, UiSize specifies the width and height with UiLength
	objects, thus allowing use of more abstract sizes that depend on the font size, etc.
    
	*/
struct UiSize
{
public:
	UiSize()
	{		
	}
        
    UiSize(const Size& size)
		: width(size.width)
		, height(size.height)
	{		
	}

	UiSize(const UiLength& width, const UiLength& height)
		: width(width)
		, height(height)
	{		
	}
	
    
    UiLength width;
	UiLength height;
};


}


inline bool operator==(const bdn::UiSize& a, const bdn::UiSize& b)
{
	return (a.width==b.width
			&& a.height==b.height);
}

inline bool operator!=(const bdn::UiSize& a, const bdn::UiSize& b)
{
	return !operator==(a, b);
}


#endif


