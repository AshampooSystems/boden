#ifndef BDN_Margin_H_
#define BDN_Margin_H_

namespace bdn
{
	

/** Represents the size of a margin or border.

	When margin components are listed individually (for example in a constructor)
	then their order is always the same as the order of the numbers on a clock:
	top, right, bottom, left

	This is also the same order that is used in the CSS standard.	
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

	explicit Margin(int all)
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



    
    Margin operator+(const Margin& o) const
    {
        return Margin(*this) += o;
    }
    
    Margin operator-(const Margin& o) const
    {
        return Margin(*this) -= o;
    }


    Margin& operator+=(const Margin& o)
    {
        top += o.top;
        right += o.right;
        bottom += o.bottom;
        left += o.left;
        
        return *this;
    }

    Margin& operator-=(const Margin& o)
    {
        top -= o.top;
        right -= o.right;
        bottom -= o.bottom;
        left -= o.left;
        
        return *this;
    }
        

	int top = 0;
	int right = 0;	
	int bottom = 0;	
	int left = 0;
};


}


inline bool operator==(const bdn::Margin& a, const bdn::Margin& b)
{
	return (a.top==b.top
			&& a.right==b.right
			&& a.bottom==b.bottom
			&& a.left==b.left);
}

inline bool operator!=(const bdn::Margin& a, const bdn::Margin& b)
{
	return !operator==(a, b);
}

#endif


