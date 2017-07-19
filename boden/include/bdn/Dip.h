#ifndef BDN_Dip_H_
#define BDN_Dip_H_

#include <map>

namespace bdn
{


/** A class that represents a value in DIP (device independent pixel) values.

    The main function of this class is to make different DIP values comparable
    in a way so that floating point rounding errors are ignored.

    The necessity for this comes from the fact that floating point calculations can
    introduce slight errors, since not all numbers can be represented exactly.

    If you calculate the same DIP value with two mathematical equivalent operations then
    the resulting floating point numbers might be slightly different. This class makes
    it easy to ignore such insignificant differences.

    The Dip class can handle infinity double values. The comparisons work as expected.

    Dip values can be implicitly converted to double values.
*/
class Dip : public Base
{
public:
    
	Dip(double val=0)
        : _value(val)
	{		
	}

    Dip(const Dip& d)
        : _value( d._value )
	{		
	}
    
    /** Compares two DIP values while ignoring insignificant differences. Returns -1 if a is significantly
        smaller, 0 if the two values are not significantly different, 1 if a is significantly bigger.

        The function also works with infinity double values. The comparisons work as expected.
        
        If any of the parameters is the special NaN value then the return value is NaN
        */
    static double compare( double a, double b)
    {
        if(std::isfinite(a) && std::isfinite(b))
        {
            double diff = a-b;
            if(diff < - significanceBoundary() )
                return -1;
            else if(diff > significanceBoundary() )
                return 1;
            else
                return 0;
        }
        else
        {
            if(a<b)
                return -1;
            else if(a>b)
                return 1;
            else if(std::isnan(a) || std::isnan(b) )
                return std::numeric_limits<double>::quiet_NaN();
            else
                return 0;
        }
    }


    /** Returns true if the specified two DIP values are "equal" in the sense that
        their differences are insignificant.*/
    static bool equal( double a, double b)
    {
        if(std::isfinite(a) && std::isfinite(b))
            return fabs(a-b) <= significanceBoundary();
        else
        {
            // when infinities are involved then slight differences do affect the outcome
            return (a==b);
        }
    }
    

    /** Returns true if the specified Size objects with Dip values are "equal" in the sense that
        their differences are insignificant.*/
    static bool equal(const Size& a, const Size& b)
    {
        return ( equal(a.width, b.width) && equal(a.height, b.height) );
    }

    /** Returns true if the specified Point objects with Dip values are "equal" in the sense that
        their differences are insignificant.*/
    static bool equal(const Point& a, const Point& b)
    {
        return ( equal(a.x, b.x) && equal(a.y, b.y) );
    }

    /** Returns true if the specified Rect objects with Dip values are "equal" in the sense that
        their differences are insignificant.*/
    static bool equal(const Rect& a, const Rect& b)
    {
        return ( equal(a.x, b.x)
                && equal(a.y, b.y)
                && equal(a.width, b.width)
                && equal(a.height, b.height) );
    }


    double getValue() const
    {
        return _value;
    }



    void operator=(double v)
    {
        _value = v;
    }

    operator double() const
    {
        return _value;
    }


    
    bool operator==(const Dip& o) const
    {
        return compare(_value, o._value) == 0;
    }

    bool operator!=(const Dip& o) const
    {
        return compare(_value, o._value) != 0;
    }


    bool operator<(const Dip& o) const
    {
        return compare(_value, o._value) < 0;
    }

    bool operator>(const Dip& o) const
    {
        return compare(_value, o._value) > 0;
    }

    bool operator<=(const Dip& o) const
    {
        return compare(_value, o._value) <= 0;
    }

    bool operator>=(const Dip& o) const
    {
        return compare(_value, o._value) >= 0;
    }
    


    bool operator==(double o) const
    {
        return compare(_value, o) == 0;
    }

    bool operator!=(double o) const
    {
        return compare(_value, o) != 0;
    }


    bool operator<(double o) const
    {
        return compare(_value, o) < 0;
    }

    bool operator>(double o) const
    {
        return compare(_value, o) > 0;
    }

    bool operator<=(double o) const
    {
        return compare(_value, o) <= 0;
    }

    bool operator>=(double o) const
    {
        return compare(_value, o) >= 0;
    }
    

    /** Returns the minimum difference between two DIP values that is considered significant.*/
    static constexpr double significanceBoundary()
    {
        // To determine which differences are "insignificant" we have to look at what
        // the value "1 DIP" represents.
        // A DIP is roughly size of a "legacy pixel" on an old school
        // 96 DPI display. 4k displays use pixels that are about 4 times smaller
        // (i.e. each pixel is roughly 0.25 DIPs).

        // Since for 4k displays individual pixels are not visible to the human eye,
        // differences below 0.1 DIP should be indiscernible on most normal displays.
        // For huge displays or close viewing distances a lower limit might apply.
        // 0.0001 seems like a good limit.

        // Floating point values have about a 15 decimal digit precision.
        // Desktop displays often have a width of 1000 - 2000 DIPs.
        // If we allow for 1000 times bigger displays (=display walls)
        // then we will need about 7 digits before the decimal point.
        // I.e. we have 8 digits after the decimal point in that case.
        
        // So our chosen 0.0001 precision is well within the range where floating
        // point numbers in the expected ranges can be reasonably accurate. The only
        // differences can come from certain numbers not being exactly representable
        // as a floating point and those differences will always be less than 0.0001 in
        // the given number range.        

        return 0.0001;
    }

private:

    double _value;
};


}



inline bool operator==(double a, const bdn::Dip& b)
{
    return b==a;
}

inline bool operator!=(double a, const bdn::Dip& b)
{
    return b!=a;
}

inline bool operator<(double a, const bdn::Dip& b)
{
    return b>a;
}

inline bool operator>(double a, const bdn::Dip& b)
{
    return b<a;
}

inline bool operator<=(double a, const bdn::Dip& b)
{
    return b>=a;
}

inline bool operator>=(double a, const bdn::Dip& b)
{
    return b<=a;
}


#endif
