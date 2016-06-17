#ifndef BDN_UiMargin_H_
#define BDN_UiMargin_H_

#include <bdn/UiLength.h>

namespace bdn
{
	

/** Represents the size of a margin or border around a UI element.

	In contast to #Margin, UiMargin specifies the margin sizes with UiLength
	objects, thus allowing use of more abstract sizes that depend on the screen
	and system settings.

	It is recommended to use UiLength::sem as the unit for margins.

	*/
struct UiMargin
{
public:
	UiMargin()
	{		
	}

	UiMargin(UiLength::Unit unit, double all)
		: top(unit, all)
		, right(unit, all)
		, bottom(unit, all)
		, left(unit, all)
	{		
	}

	UiMargin(UiLength::Unit unit, double topBottom, double leftRight)
		: top(unit, topBottom)
		, right(unit, leftRight)
		, bottom(unit, topBottom)
		, left(unit, leftRight)
	{		
	}

	UiMargin(UiLength::Unit unit, double top, double right, double bottom, double left)
		: top(unit, top)
		, right(unit, right)
		, bottom(unit, bottom)
		, left(unit, left)
	{		
	}


	UiMargin(const UiLength& all)
		: top(all)
		, right(all)
		, bottom(all)
		, left(all)
	{		
	}

	UiMargin(const UiLength& topBottom, const UiLength& leftRight)
		: top(topBottom)
		, right(leftRight)
		, bottom(topBottom)
		, left(leftRight)
	{		
	}
	
	UiMargin(const UiLength& top, const UiLength& right, const UiLength& bottom, const UiLength& left)
		: top(top)
		, right(right)
		, bottom(bottom)
		, left(left)
	{		
	}


	bool operator==(const UiMargin& o) const
	{
		return (top==o.top && right==o.right && bottom==o.bottom && left==o.left);
	}

	bool operator!=(const UiMargin& o) const
	{
		return !operator==(o);
	}

	UiLength top;
	UiLength right;
	UiLength bottom;
	UiLength left;
};


}


#endif


