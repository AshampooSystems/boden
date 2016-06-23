#ifndef BDN_Rect_H_
#define BDN_Rect_H_


#include <bdn/Margin.h>
#include <bdn/Size.h>

namespace bdn
{
	

/** Represents the position and size of a simple two dimensional
	rectangle.
	
	*/
struct Rect
{
public:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	Rect()
	{		
	}

	Rect(int x, int y, int width, int height)
		: x(x)
		, y(y)
		, width(width)
		, height(height)
	{		
	}


	/** Decrease the rect size by subtracting the specified margin.*/
	Rect operator-(const Margin& margin) const
	{
		return Rect(*this) -= margin;
	}

	/** Decrease the rect size by subtracting the specified margin.*/
	Rect& operator-=(const Margin& margin)
	{
		x += margin.left;
		y += margin.top;
		width -= margin.left + margin.right;
		height -= margin.top + margin.bottom;

		return *this;
	}


	/** Increase the rect size by adding the specified margin.*/
	Rect operator+(const Margin& margin) const
	{
		return Rect(*this) += margin;
	}

	/** Increase the rect size by adding the specified margin.*/
	Rect& operator+=(const Margin& margin)
	{
		x -= margin.left;
		y -= margin.top;
		width += margin.left + margin.right;
		height += margin.top + margin.bottom;

		return *this;
	}

	
};


}


inline bool operator==(const bdn::Rect& a, const bdn::Rect& b)
{
	return (a.x==b.x
			&& a.y==b.y
			&& a.width==b.width
			&& a.height==b.height);
}

inline bool operator!=(const bdn::Rect& a, const bdn::Rect& b)
{
	return !operator==(a, b);
}


#endif