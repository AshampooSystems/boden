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

    double top = 0;
	double right = 0;	
	double bottom = 0;	
	double left = 0;

	Margin()
		: top(0)
		, right(0)
		, bottom(0)
		, left(0)
	{		
	}

	explicit Margin(double all)
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
        
	/** Returns a locale-independent string representation of the margin.
		*/
	String toString() const
	{
		return bdn::toString(top) +", "+ bdn::toString(right) +", "+ bdn::toString(bottom) +", "+ bdn::toString(left);
	}

	
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




/** Returns true if a's fields are each smaller than b's */
inline bool operator<(const bdn::Margin& a, const bdn::Margin& b)
{
	return (a.top<b.top
			&& a.right<b.right
            && a.bottom<b.bottom
            && a.left<b.left
            );
}


/** Returns true if a's fields are each smaller or equal to b's */
inline bool operator<=(const bdn::Margin& a, const bdn::Margin& b)
{
    return (a.top<=b.top
			&& a.right<=b.right
            && a.bottom<=b.bottom
            && a.left<=b.left
            );
}


/** Returns true if a's fields are each bigger than b's */
inline bool operator>(const bdn::Margin& a, const bdn::Margin& b)
{
    return (a.top>b.top
			&& a.right>b.right
            && a.bottom>b.bottom
            && a.left>b.left
            );
}

/** Returns true if a's fields are each bigger or equal to b's */
inline bool operator>=(const bdn::Margin& a, const bdn::Margin& b)
{
    return (a.top>=b.top
			&& a.right>=b.right
            && a.bottom>=b.bottom
            && a.left>=b.left
            );
}

#endif


