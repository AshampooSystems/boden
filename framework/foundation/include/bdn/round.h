#pragma once

#include <bdn/InvalidArgumentError.h>

#include <cmath>

namespace bdn
{

    enum class RoundType
    {
        /** Round to the nearest number. Halfway values are rounded away from
           zero.*/
        nearest,

        /** Round up, to the next bigger number. Negative values are rounded
           toward zero.*/
        up,

        /** Round down, to the next smaller number. Negative values are rounded
           away from zero.*/
        down
    };

    /** Performs a stable, scaled rounding operation to the nearest scaled
       rounding boundary. Halfway< values are rounded away from zero.

        This function works analogously to stableScaledRoundUp(), except that it
       rounds to the nearest boundary. See stableScaledRoundUp() for more
       information.*/
    inline double stableScaledRoundNearest(double value, double scaleFactor)
    {
        if (scaleFactor == 0) {
            throw InvalidArgumentError("stableScaledRoundNearest called with zero scaleFactor");
        }

        // rounding to the nearest boundary is always stable. So we can simply
        // round directly.
        return std::round(value * scaleFactor) / scaleFactor;
    }

    /** User internally - not part of the public API.

        Returns true if the specified value can be considered to lie on a scaled
       rounding boundary for the specified scale factor (see
       stableScaledRoundUp() ).

        Since floating point numbers cannot represent all numbers exactly, there
       can be cases where a number needs to be represented with a value that is
       slightly bigger or smaller than the exact one. This function checks if
       the value parameter is within this range of "fuzzyness" near a rounding
       boundary for the specified scale factor.
        */
    inline bool _isOnScaledRoundingBoundary(double value, double scaleFactor)
    {
        double closestBoundary = stableScaledRoundNearest(value, scaleFactor);
        if (value > closestBoundary) {
            if (std::nexttoward(value, value - 1) < closestBoundary) {
                // the previous representable number is on the other side of the
                // boundary.
                // => we can consider the input to already be on the boundary.
                return true;
            }
        } else if (value < closestBoundary) {
            if (std::nexttoward(value, value + 1) > closestBoundary) {
                // the next representable number is on the other side of the
                // boundary.
                // => we can consider the input to already be on the boundary.
                return true;
            }
        } else {
            // => already rounded.
            return true;
        }

        return false;
    }

    /** Performs a stable, scaled rounding operation to the next highest
       boundary. Negative numbers are rounded toward zero.

        In contrast to normal integer rounding, scaled rounding can be used to
       round to a number grid with a non-integer step size. The scaleFactor
       parameter indicates the scale factor from the rounding grid to integer
       numbers. So a scale factor of 2 would mean that there are 2 rounding
       boundaries per integer, i.e. the rounding grid is in 0.5 increments (0,
       0.5, 1, 1.5, etc.). The function will round to the next higher rounding
       boundary.

        The term "stable" means that when the result (!) of the rounding
       operation is passed to the same rounding function again then you get the
       exact same result. This might seem to be obvious, but when a scale factor
       is involved then the limitations of floating point numbers can mean that
       the exact mathematical result of the rounding operation cannot be
       represented exactly. In these cases the floating point system will return
       an imperceptibly smaller or larger value. If a slightly larger value is
       returned then passing it to a native up-rounding operation would
       ordinarily bump it to the NEXT boundary. This does not happen with a
       "stable" implementation like this one. The slight inexactness is detected
       and the rounding result is the same boundary.
    */
    inline double stableScaledRoundUp(double value, double scaleFactor)
    {
        if (scaleFactor == 0) {
            throw InvalidArgumentError("stableScaledRoundUp called with zero scaleFactor");
        }

        if (_isOnScaledRoundingBoundary(value, scaleFactor)) {
            // we can consider the number to be "on" the rounding boundary. Do
            // not round again.
            return value;
        }
        {
            return std::ceil(value * scaleFactor) / scaleFactor;
        }
    }

    /** Performs a stable, scaled rounding operation, down to the next smaller
       scaled rounding boundary. Negative numbers are rounded away from zero.

        This function works analogously to stableScaledRoundUp(), except that it
       rounds down, not up. See stableScaledRoundUp() for more information.*/
    inline double stableScaledRoundDown(double value, double scaleFactor)
    {
        if (scaleFactor == 0) {
            throw InvalidArgumentError("stableScaledRoundDown called with zero scaleFactor");
        }

        if (_isOnScaledRoundingBoundary(value, scaleFactor)) {
            // we can consider the number to be "on" the rounding boundary. Do
            // not round again.
            return value;
        }
        {
            return std::floor(value * scaleFactor) / scaleFactor;
        }
    }

    /** Performs a stable, scaled rounding operation. The rounding type is
       indicated by the roundType parameter - it controls whether the function
       behaves like stableScaledRoundUp(), stableScaledRoundDown() or
       stableScaledRoundNearest.

        See stableScaledRoundUp() for more information.

        */
    inline double stableScaledRound(RoundType roundType, double value, double scaleFactor)
    {
        if (roundType == RoundType::down) {
            return stableScaledRoundDown(value, scaleFactor);
        }
        if (roundType == RoundType::up) {
            return stableScaledRoundUp(value, scaleFactor);
        }
        return stableScaledRoundNearest(value, scaleFactor);
    }
}
