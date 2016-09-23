#ifndef BDN_RoundNearest_H_
#define BDN_RoundNearest_H_

#include <bdn/Rounder.h>
#include <cmath>

namespace bdn
{

/** Rounds to the nearest integer value.
    Halfway cases (i.e. .5) are rounded away from zero.
    
    Behaves the same way as std::round.
*/
inline double roundNearest(double value)
{
    return std::round(value);
}


/** Rounds numbers and various basic types (like Size, Rect, etc.) to
    arbítrary boundaries using a standard "round to nearest" strategy.

    The rounding works the same way as in std::round.

    The Rounder class takes one parameter in its constructor: the unit.
    It specifies the basic unit to round to. All rounded output values will end up being
    a multiple of this value.
    
    For example, to round to full integers specify 1.

    To round to 2 decimal places specify 0.01.

    You can also specify arbitrary numbers like 12.3456.
    Then all output values will be rounded to a multiple of 12.3456.

    The rounding is guaranteed to be stable. I.e. if rounding a previous round result
    again returns the same value.    

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
typedef Rounder<roundNearest> RoundNearest;


}

#endif
