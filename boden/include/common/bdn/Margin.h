#ifndef BDN_Margin_H_
#define BDN_Margin_H_

namespace bdn
{
	

/** Represents the size of a margin or border.
	
	Note that the components are floating point numbers (double), not integers.
	*/
struct Margin
{
public:
	Margin()
		: top(0)
		, right(0)
		, bottom(0)
		, left(0)
	{		
	}

	Margin(double all)
	{
		top = all;
		right = all;
		bottom = all;
		left = all;		
	}

	Margin(double topBottom, double leftRight)
	{
		top = topBottom;
		right = leftRight;
		bottom = topBottom;
		left = leftRight;		
	}

	Margin(double top, double right, double bottom, double left)
	{
		this->top = top;
		this->right = right;
		this->bottom = bottom;
		this->left = left;
	}

	double top = 0;
	double right = 0;	
	double bottom = 0;	
	double left = 0;
};


}


#endif


