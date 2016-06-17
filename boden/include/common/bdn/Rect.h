#ifndef BDN_Rect_H_
#define BDN_Rect_H_


#include <bdn/Margin.h>

namespace bdn
{
	

/** Represents the position and size of a simple two dimensional
	rectangle.
	
	Note that the components are floating point numbers (double), not integers.
	*/
struct Rect
{
public:
	double x = 0;
	double y = 0;
	double width = 0;
	double height = 0;

	Rect()
	{		
	}

	Rect(double x, double y, double width, double height)
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
		x -= margin.left;
		y -= margin.top;
		width -= margin.left + margin.right;
		height -= margin.top + margin.bottom;
	}


	/** Increase the rect size by adding the specified margin.*/
	Rect operator+(const Margin& margin) const
	{
		return Rect(*this) += margin;
	}

	/** Increase the rect size by adding the specified margin.*/
	Rect& operator+=(const Margin& margin)
	{
		x += margin.left;
		y += margin.top;
		width += margin.left + margin.right;
		height += margin.top + margin.bottom;
	}


	bool operator==(const Rect& o) const
	{
		return (x==o.x && y==o.y && width==o.width && height==o.height);
	}

	bool operator!=(const Rect& o) const
	{
		return !operator==(o);
	}
};


}


#endif