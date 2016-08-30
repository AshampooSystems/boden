#ifndef BDN_Size_H_
#define BDN_Size_H_

#include <bdn/Margin.h>

namespace bdn
{
	

/** Represents the a size (width and height).
	
	*/
struct Size
{
public:
	int width = 0;
	int height = 0;

	Size()
	{		
	}

	Size(int width, int height)
		: width(width)
		, height(height)
	{		
	}


	/** Subtracts the size of the specified margin from the size.*/
	Size operator-(const Margin& margin) const
	{
		return Size(*this) -= margin;
	}

	/** Subtracts the size of the specified margin from the size.*/
	Size& operator-=(const Margin& margin)
	{
		width -= margin.left + margin.right;
		height -= margin.top + margin.bottom;

		return *this;
	}


	/** Adds the size of the specified margin to the size.*/
	Size operator+(const Margin& margin) const
	{
		return Size(*this) += margin;
	}

	/** Adds the size of the specified margin to the size.*/
	Size& operator+=(const Margin& margin)
	{
		width += margin.left + margin.right;
		height += margin.top + margin.bottom;

		return *this;
	}


	/** Adds the specified size to this size.*/
	Size operator+(const Size& o) const
	{
		return Size(*this) += o;
	}

	/** Adds the specified size to this size.*/
	Size& operator+=(const Size& o)
	{
		width += o.width;
		height += o.height;

		return *this;
	}


	/** Subtracts the specified size from this size.*/
	Size operator-(const Size& o) const
	{
		return Size(*this) -= o;
	}

	/** Subtracts the specified size from this size.*/
	Size& operator-=(const Size& o)
	{
		width -= o.width;
		height -= o.height;

		return *this;
	}
	
};


}


inline bool operator==(const bdn::Size& a, const bdn::Size& b)
{
	return (a.width==b.width
			&& a.height==b.height);
}

inline bool operator!=(const bdn::Size& a, const bdn::Size& b)
{
	return !operator==(a, b);
}

/** Returns true if a's width and height are each smaller than b's */
inline bool operator<(const bdn::Size& a, const bdn::Size& b)
{
	return (a.width<b.width
			&& a.height<b.height);
}


/** Returns true if a's width and height are each smaller or equal to b's */
inline bool operator<=(const bdn::Size& a, const bdn::Size& b)
{
	return (a.width<=b.width
			&& a.height<=b.height);
}


/** Returns true if a's width and height are each bigger than b's */
inline bool operator>(const bdn::Size& a, const bdn::Size& b)
{
	return (a.width>b.width
			&& a.height>b.height);
}

/** Returns true if a's width and height are each bigger or equal to b's */
inline bool operator>=(const bdn::Size& a, const bdn::Size& b)
{
	return (a.width>=b.width
			&& a.height>=b.height);
}



#endif

