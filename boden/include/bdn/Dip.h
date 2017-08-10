#ifndef BDN_Dip_H_
#define BDN_Dip_H_

#include <map>

#include <bdn/round.h>

namespace bdn
{


/** A class that represents a value in DIP (device independent pixel) values.

    The main function of this class is to make different DIP values comparable
    in a way so that invisibly small differences are ignored.
    
    

    The rationale for this comes from two main problems:

    1) Floating point inaccuracies: Floating point calculations can
       introduce slight errors, since not all numbers can be represented exactly.

       If you calculate the same DIP value with two mathematical equivalent operations then
       the resulting floating point numbers might be slightly different. This class makes
       it easy to ignore such insignificant differences.

       So íf you want repeatable and consistent calculations then it can be helpful
       to ignore tiny differences.


    2) There are some cases in which differences between values that are bigger than
       the tiny floating point inaccuracies should also be ignored. For example, when
       you want to round a value expressed in DIPs to integer multiples of physical pixels.
       
       Let's look at an example: let's assume in this example that physical pixels have a
       size of 0.5 DIPs (i.e. there are 2 pixels per DIP). Let's say you have a button and the
       button reports that it needs to be 10.01 DIPs wide to show its content.
       When you round sizes to physical pixels then one usually wants to round up, to make sure
       that the entire content is displayed. If you round up from 10.01 to the next pixel
       then you will get a result of 10.5.
       However, 10.01 is visually indistinguishable from 10.0. I.e. if one were to reduce the
       size of the button by 0.01 then no visible clipping to the content would occur.
       So it would be much more reasonable to round this value down to 10.0 instead of
       up to 10.5. And this is exactly where the DIP class comes in: it defines the boundary
       at which we consider a difference to be "visible" and it provides functionality that
       treats smaller differences than this as non-existant. I.e. the Dip class would consider
       10.01 to be equal to 10.0, thus it would NOT round up.


    Note that the Dip class can handle infinity double values. The comparisons work as expected.
    Most other operations will return infinity as the output when you put in an infinity value.

    Dip objects can be implicitly converted to double values.

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


    /** Aligns the aligns of the Dip object to a physical pixel boundary.
        See pixelAlignValue() for more information.*/
    Dip& pixelAlign(double pixelsPerDip, RoundType roundType)
    {
        _value = pixelAlign(_value, pixelsPerDip, roundType);
        return *this;
    }


    /** Aligns a value in the DIP units to physical pixel boundaries.

        pixelsPerDip indicates the number of physical pixels per DIP unit.
        For example, if 1 DIP corresponds to 2 pixels, then the factor would be 2.
        The factor does NOT have to be an integer.

        roundType indicates the rounding strategy (up, down or nearest).
        
        Important: this function uses specialized rounding behaviour that works on the notion of
        "visible differences", rather than only pure hard numbers.

        In practice that means that DIP values that are VERY close to a pixel boundary,
        to the point where the difference is not noticeable, are
        always rounded towards that pixel boundary. Even if the specified rounding type is not "nearest".

        For example, if you have a DIP value of 3.701 and a pixel boundary would be at 3.7,
        then 3.701 is always rounded down to 3.7, even if the rounding type is "nearest" or "up".

        If the difference is bigger (let's say 3.72) then the rounding type determines in which
        direction one will round. If you specify a rounding type of "up" then 3.72 will be rounded to the next higher pixel
        boundary.

        Why is this useful?
        ------------------

        The rationale for this special behaviour is that coordinates and sizes are represented with double values
        and floating point calculations can always introduce slight errors.
        There is the potential that if you make a calculation in one way and then do the same calculation in another,
        mathematically equivalent, way then you can end up with a slightly different floating point result.    
        If you are calculating with coordinates then the first calculation might give you
        a result value that is slightly below a physical pixel boundary and the second calculation might give you a 
        result value that is slightly above the pixel boundary. If you then apply rounding (let's say with the
        "up" rounding type) then you would get a 1 pixel difference as a result.

        Note that such rounding differences could also occur when you use the "nearest" rounding type.
        If one calculation gives you a value slightly below the halfway point between two pixels
        and the other gives you a value slightly above it then you have the same effect and get a 
        1 pixel difference.

        The pixelAlign function solves these issues by considering two values to be equal, if their difference
        would not be "visible".
    
        The notion of "visible difference" is obviously hard to define in an objective way.
        A subjective definition would be that if you made two tiny dots on the glass of your monitor with a pen,
        one at the one DIP value and below that one at the other DIP value, would you see a difference?
    
        In our example, if you made a dot at x=3.7 and below that one at x=3.701, they would look exactly the same to the human eye.

        Which distance is considered to be the boundary between visible or invisible is obviously highly
        subjective. pixelAlign() uses the value visibilityBoundary().
        */
    static double pixelAlign(double val, double pixelsPerDip, RoundType roundType)
    {
        // first we need to check if the value is "invisibly close" to a pixel boundary
        // (i.e. less than visibilityBoundary away from a pixel boundary).
        double boundaryBelow = stableScaledRound(RoundType::down, val, pixelsPerDip);
        double boundaryAbove = stableScaledRound(RoundType::up, val, pixelsPerDip);

        if( fabs(val-boundaryBelow) < visibilityBoundary() )
            return boundaryBelow;
        else if( fabs(boundaryAbove-val) < visibilityBoundary() )
            return boundaryAbove;

        if( roundType==RoundType::nearest )
        {
            // if we use "nearest" then we consider all values that are visibly
            // close to the halfway point to be on the halfway point.
            // Why do we do that? Again, this is to improve consistency for floating point
            // calculations whose mathematically exact result is exactly the halfway point.
            // This is not that uncommon: half pixel values can easily happen.
            // For example, if 1 pixel has a size of 0.8 DIPs, then 2.0 DIPs is exactly 
            // a halfway point between the pixel boundaries at 1.6 and 2.4. 
            // For these kinds of results it would be bad if one calculation results in 1.99999999
            // and another in 2.000000001 due to rounding errors and then the result would
            // be a 1 pixel difference. So we also use the "visible difference" measure
            // for the halfway point.

            // Note that this of course creates a new boundary slightly below and above the halfway
            // point, where tiny differences can result in a 1 pixel rounding difference.
            // However, calculation results that fall exactly on such boundaries should be much less common,
            // since the values are much smaller and as such less likely to occur in values defined by humans.
            if( fabs( fabs(boundaryAbove+boundaryBelow)/2 - fabs(val) ) < visibilityBoundary() )
            {
                // boundaryAbove is closer to zero than boundaryBelow for negative number.
                // We must return the one that is furthest away from zero
                return (val<0) ? boundaryBelow : boundaryAbove;
            }
        }

        // the value is not close to a pixel boundary. Use normal rounding.            
        return stableScaledRound(roundType, val, pixelsPerDip);
    }

    /** Aligns the specified rectangle in DIP coordinates to physical pixel boundaries. positionRoundType indicates how the position
        is rounded, sizeRoundType indicates how the size is rounded.
        
        See pixelAlign() for more information about how this alignment is done.
        
        */
    static Rect pixelAlign(const Rect& rect, double pixelsPerDip, RoundType positionRoundType, RoundType sizeRoundType)
    {
        return Rect( pixelAlign(rect.x, pixelsPerDip, positionRoundType),
                     pixelAlign(rect.y, pixelsPerDip, positionRoundType),
                     pixelAlign(rect.width, pixelsPerDip, sizeRoundType),
                     pixelAlign(rect.height, pixelsPerDip, sizeRoundType) );
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
            if(diff < - visibilityBoundary() )
                return -1;
            else if(diff > visibilityBoundary() )
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
            return fabs(a-b) <= visibilityBoundary();
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


    /** Returns the distance in DIPs at which a difference between two values
        is considered to be visible to the naked eye.

        Background information about why this value was chosen
        ------------------------------------------------------

        There is a whole spectrum of values between what is "definitely visible"
        and "definitely invisible". The spectrum is quite big, as it depends on
        many external conditions (display, person, lighting, contrast, etc.).
        So a value like this is always a little arbitrary.
        
        The value was deliberately chosen to be quite small, so
        that one can be reasonably sure that a difference that is smaller than this
        will indeed be unnoticeable in most circumstances.
                
        Note that this value was also deliberately chosen to be a weird, uneven number that is
        unlikely to occur in values chosen by humans. The reason for this is that floating
        point calculations whose results are close to the visibility boundary would again
        have the same floating point inaccuracy problem. If results come close to this then
        small, mathematically equivalent differences in the formula could cause a result
        to switch from visible to invisible difference. Since we want to avoid such cases, we chose
        a weird value that is unlikely to occur in "normal life".
        */
    static constexpr double visibilityBoundary() 
    {
        // To determine which differences are "insignificant" we have to look at what
        // the value "1 DIP" represents.
        // A DIP is roughly size of a "legacy pixel" on an old school
        // 96 DPI display. 4k displays use pixels that are about 4 times smaller
        // (i.e. each pixel is roughly 0.25 DIPs).

        // Since for 4k displays individual pixels are considered to be invisible to the human eye,
        // differences below 0.25 DIP should be indiscernible on most normal displays.

        // However, this only applies to desktop displays. If you have much bigger displays
        // (e.g. in a football stadium) then the DIP unit SHOULD be redefined so that 
        // one DIP again has roughly the same apparent subjective size at the "normal" vieweing
        // distance. However, the vieweing distance can have a much wider range for such specialty
        // displays.
        // So we want to have a little bit more wriggle room and choose a value that is significantly
        // smaller than the "normal" visibility boundary for desktop displays.

        // we use a value around 0.04 (again 6 times smaller than the 4k resolution pixel).
        return 0.03719;
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
