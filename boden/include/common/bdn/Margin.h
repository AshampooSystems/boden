#ifndef BDN_Margin_H_
#define BDN_Margin_H_

namespace bdn
{
	

/** Represents the size of a margin or border.
	
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

	Margin(int all)
	{
		top = all;
		right = all;
		bottom = all;
		left = all;		
	}

	Margin(int topBottom, int leftRight)
	{
		top = topBottom;
		right = leftRight;
		bottom = topBottom;
		left = leftRight;		
	}

	Margin(int top, int right, int bottom, int left)
	{
		this->top = top;
		this->right = right;
		this->bottom = bottom;
		this->left = left;
	}


	int top = 0;
	int right = 0;	
	int bottom = 0;	
	int left = 0;
};


}


#endif


