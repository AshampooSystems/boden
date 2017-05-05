#ifndef BDN_Size_H_
#define BDN_Size_H_

#include <bdn/Margin.h>

#include <limits>

namespace bdn
{
	

/** Represents the a size (width and height).
	
	*/
struct Size
{
public:

    /** Returns a number that can be used as a Size component (width and/or height)
        to indicate a value that is intentionally left unspecified.

        Arithmetic operations where one side is componentNone always result in componentNone.

        componentNone is often used to indicate "unlimited" and similar special circumstances (for example
        when size limits are represented with bdn::Size object.

        The value returned by componentNone() can be used in normal equality comparisons,
        i.e. it does *not* behave like the NaN floating point value).

        The actual value of componentNone is the positive inifinity floating point value.

        Also see none().
        */
    static constexpr double componentNone()
    {
        return std::numeric_limits<double>::infinity();
    }


    /** Returns a Size object in which both width and height are intentionally left unspecified
        (they both have the value componentNone() ).

        Arithmetic operations where one side is none always result in none.

        none is often used to indicate "unlimited" and similar special circumstances (for example
        when size limits are represented with bdn::Size object.

        Also see componentNone().
        */
    static constexpr Size none()
    {
        return Size(componentNone(), componentNone() );
    }


	double width = 0;
	double height = 0;

	constexpr Size()
	{		
	}

	constexpr Size(double width, double height)
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


    /** Applies a minimum and maximum size constraint to the Size object.

        For example, minSize.width is a valid finite value (i.e. not componentNone(), infinity or NaN)
        and the width than minSize.width then it is set to minSize.width.

        In the same way, maxSize specifies the upper limit. If one of the Size components
        is bigger than the corresponding maxSize component then it is set to the maxSize value.

        You can pass Size::none() to minSize and/or maxSize to indicate that there should be no
        limit in that direction. You can also set individual components of the minSize and maxSize
        to Size::componentNone() to indicate that only that limit component should be ignored and that for
        the component there should be no limit in that direction.

        */
    void applyConstraints(const Size& minSize, const Size& maxSize)
    {
        if( std::isfinite(minSize.width) && width < minSize.width)
            width = minSize.width;            

        if( std::isfinite(minSize.height) && height < minSize.height)
            height = minSize.height;            

        if( std::isfinite(maxSize.width) && width > maxSize.width)
            width = maxSize.width;  

        if( std::isfinite(maxSize.height) && height > maxSize.height )
            height = maxSize.height;  
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

