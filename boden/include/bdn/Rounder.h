#ifndef BDN_Rounder_H_
#define BDN_Rounder_H_

#include <cmath>

namespace bdn
{


/** Rounds numbers and various basic types (like Size, Rect, etc.) to
    arbítrary boundaries with an arbitrary rounding function.

    There are convenience versions of this that are less complicated to use:
    RoundNearest, RoundUp and RoundDown. It is recommended to use those if you
    need standard rounding functions.
    
    The Rounder class takes one parameter in its constructor: the unit.
    It specifies the basic unit to round to. All rounded output values will end up being
    a multiple of this value.
    
    For example, to round to full integers specify 1.

    To round to 2 decimal places specify 0.01.

    You can also specify arbitrary numbers like 12.3456.
    Then all output values will be rounded to a multiple of 12.3456.

    The rounding is guaranteed to be stable. I.e. if rounding a previous round result
    again returns the same value.

    The Rounder object also provides a call operator. So instead of calling the round()
    function you can also call the object itself (see example).
    
    The template parameter RoundFunction is used to specify the type of rounding.
    It must be a function that takes a double argument and rounds it to an integer
    value with the desired algorithm. Pass std::round to round to the nearest unit,
    std::ceil to round UP to the next unit and std::floor to round DOWN to the next unit.

    Example:

    \code

    // round UP to two decimal places
    Rounder<std::ceil> upRounder(0.01);
    
    upRounder.round(1.234);
    // returns is 1.24

    // you can also use the call operator
    upRounder(1.234);
    // returns the same as calling round (1.24)

    // to round DOWN:
    Rounder<std::floor> downRounder(0.01);

    // to round to nearest:
    Rounder<std::round> nearestRounder(0.01);


    // shortest way to use:
    result = Rounder<std::ceil>(0.01)(1.234);

    \endcode
*/
template< double (*roundFunction)(double) >
class Rounder : public Base
{
public:
    explicit Rounder(double unit)
	{		
        if(unit==0)
            throw InvalidArgumentError("Rounder constructor called with zero unit.");
        _unit = unit;
	}


    /** Returns the Rounder's unit (see Rounder class documentation).*/
    double getUnit() const
    {
        return unit;
    }



    /** Rounds a double value.*/
    double round(double val) const
    {
        // we want the rounding to be stable.
        // I.e. round( round(val) ) == round(val).
        // That seems to be intuitively true, but in the world of floating
        // point arithmetic it is not.
        // The division and multiplication by _unit can introduce a minute 
        // difference due to the fact that floating point values sometimes cannot
        // represent the number exactly.
        // Consider the case where we use the std::ceil rounding function.
        // If the divide-multiply cycle creates a slight increase in the number
        // then rounding a rounded result will cause us to go up another unit.

        // So to be stable we have to detect if the input value is already "rounded enough".
        // We do that by comparing the next and previous representable floating point numbers
        // to the closes multiple of unit. If one of those is "on the other side" of the round result
        // then we know that we have the kind of minute deviation described above.

        double valUnits = val/_unit;

        double closestUnitMultiple = std::round(valUnits)*_unit;
        if(val > closestUnitMultiple)
        {
            if( std::nexttoward(val, val-1) < closestUnitMultiple )
            {
                // the previous representable number is on the other side of the unit multiple.
                // => we can consider the input to already be a multiple of the unit.
                return val;
            }
        }
        else if(val < closestUnitMultiple)
        {
            if( std::nexttoward(val, val+1) > closestUnitMultiple )
            {
                // the next representable number is on the other side of the the unit multiple..
                // => we can consider the input to already be a multiple of the unit.
                return val;
            }
        }
        else
        {
            // => already rounded.
            return val;
        }
            
        return roundFunction(valUnits) * _unit;
    }

    double operator()(double val) const
    {
        return round(val);
    }



    /** Rounds a Size object by rounding its width and height fields.*/
    Size round(const Size& size) const
    {
        return Size(
            round(size.width),
            round(size.height) );
    }

    Size operator()(const Size& size) const
    {
        return round(size);
    }


    /** Rounds a Size object by rounding its x and y fields.*/
    Point round(const Point& point) const
    {
        return Point(
            round(point.x),
            round(point.y) );
    }

    Point operator()(const Point& point) const
    {
        return round(point);
    }

    /** Rounds a Rect object by rounding its x, y, width and height fields.*/
    Rect round(Rect rect) const
    {
        return Rect(
            round(rect.x),
            round(rect.y),
            round(rect.width),
            round(rect.height)            
            );
    }

    Rect operator()(const Rect& rect) const
    {
        return round(rect);
    }


    /** Rounds a Margin object by rounding its top, right, bottom and left fields.*/
    Margin round(Margin margin) const
    {
        return Margin(
            round(margin.top),
            round(margin.right),
            round(margin.bottom),
            round(margin.left)
            );
    }

    Margin operator()(const Margin& margin) const
    {
        return round(margin);
    }


private:
    double _unit;
};
	

}

#endif
