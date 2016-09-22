#ifndef BDN_RoundNearest_H_
#define BDN_RoundNearest_H_

#include <bdn/Rounder.h>
#include <cmath>

namespace bdn
{


/** Rounds numbers and various basic types (like Size, Rect, etc.) to
    arbítrary boundaries using a standard "round to nearest" strategy.

    The Rounder class takes one parameter in its constructor: the unit.
    It specifies the basic unit to round to. All rounded output values will end up being
    a multiple of this value.
    
    For example, to round to full integers specify 1.

    To round to 2 decimal places specify 0.01.

    You can also specify arbitrary numbers like 12.3456.
    Then all output values will be rounded to a multiple of 12.3456.
    
    The template parameter RoundFunction is used to specify the type of rounding.
    It must be a function that takes a double argument and rounds it to an integer
    value with the desired algorithm. Pass std::round to round to the nearest unit,
    std::ceil to round UP to the next unit and std::floor to round DOWN to the next unit.


    Example:

    \code

    // round to two decimal places
    RoundNearest r(0.01);
    
    result = r.round(1.234);
    // returns is 1.23

    // you can also use the call operator
    result = r(1.234);
    // returns the same as calling round (1.23)

    // shortest way to use:
    result = RoundNearest(0.01)(1.234);
    
    \endcode
*/
typedef class RoundNearest : public Rounder<std::round>
{
public:
    explicit RoundNearest(double unit)
        : Rounder(unit)
	{
	}

};
	

}

#endif
