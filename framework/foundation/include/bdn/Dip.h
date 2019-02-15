#pragma once

#include <map>

#include <bdn/Base.h>
#include <bdn/Rect.h>
#include <bdn/round.h>

namespace bdn
{

    /** A class that represents a value in DIP (device independent pixel)
       values.

        The main function of this class is to make different DIP values
       comparable in a way so that insignificantly small differences are
       ignored.

        The rationale for this comes from the fact that floating point
       calculations can introduce slight errors, since not all numbers can be
       represented exactly.

        If you calculate the same DIP value with two mathematical equivalent
       operations then the resulting floating point numbers might be slightly
       different. So íf you want repeatable and consistent calculations then it
       can be helpful to ignore tiny differences.

        So, what is an "insignificant difference"? The static function
       significanceBoundary() returns the minimum difference that is considered
       to be significant. See the documentation of significanceBoundary() for
       more information.

        Dip objects can be implicitly converted to double values.

        Infinities and NaN
        ------------------

        Note that the Dip class can also handle the special infinity and NaN
       floating point values. Dip comparison operations involving these special
       values behave the same way as normal floating point comparisons with
       these special values would.

        However, note that some C++ compilers have options that enable
       aggressive floating point optimizations (for example -ffast-math for
       Clang). When such optimizations are activated then floating operations
       involving infinities or NaN do not work correctly on the compiler level.
        Consequently, Dip operations with these special values will also not
       work correctly when such optimizations are active.

        Also see \ref BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS

    */
    class Dip : public Base
    {
      public:
        Dip(double val = 0) : _value(val) {}

        Dip(const Dip &d) : _value(d._value) {}

        /** Aligns the aligns of the Dip object to a physical pixel boundary.
            See pixelAlign(double, double, RoundType) for more information.*/
        Dip &pixelAlign(double pixelsPerDip, RoundType roundType)
        {
            _value = pixelAlign(_value, pixelsPerDip, roundType);
            return *this;
        }

        /** Aligns a value in the DIP units to physical pixel boundaries.

            pixelsPerDip indicates the number of physical pixels per DIP unit.
            For example, if 1 DIP corresponds to 2 pixels, then the factor would
           be 2. The factor does NOT have to be an integer.

            roundType indicates the rounding strategy (up, down or nearest).

            Important: this function uses specialized rounding behaviour that
           uses the concept of "insignificant differences" (see class
           description).

            In practice that means that DIP values that are very close to a
           pixel boundary always rounded towards that pixel boundary. Even if
           the specified rounding type is not "nearest".

            For example, if you have a DIP value of 3.701 and a pixel boundary
           would be at 3.7, then 3.700001 is always rounded down to 3.7, even if
           the rounding type is "nearest" or "up".

            If the difference is bigger (let's say 3.72) then the rounding type
           determines in which direction one will round. If you specify a
           rounding type of "up" then 3.72 will be rounded to the next higher
           pixel boundary.

            Why is this useful?
            ------------------

            The rationale for this special behaviour is that coordinates and
           sizes are represented with double values and floating point
           calculations can always introduce slight errors. There is the
           potential that if you make a calculation in one way and then do the
           same calculation in another, mathematically equivalent, way then you
           can end up with a slightly different floating point result. If you
           are calculating with coordinates then the first calculation might
           give you a result value that is slightly below a physical pixel
           boundary and the second calculation might give you a result value
           that is slightly above the pixel boundary. If you then apply rounding
           (let's say with the "up" rounding type) then you would get a 1 pixel
           difference as a result.

            Note that such rounding differences could also occur when you use
           the "nearest" rounding type. If one calculation gives you a value
           slightly below the halfway point between two pixels and the other
           gives you a value slightly above it then you have the same effect and
           get a 1 pixel difference.

            The pixelAlign function solves these issues by considering two
           values to be equal, if their difference is "insignifant" (see
           significanceBoundary() ).
            */
        static double pixelAlign(double val, double pixelsPerDip, RoundType roundType)
        {
            if (!std::isfinite(val))
                return val;

            // first we need to check if the value is "invisibly close" to a
            // pixel boundary (i.e. less than significanceBoundary away from a
            // pixel boundary).
            double boundaryBelow = std::floor(val * pixelsPerDip) / pixelsPerDip;
            double boundaryAbove = std::ceil(val * pixelsPerDip) / pixelsPerDip;

            if (equal(val, boundaryBelow))
                return boundaryBelow;

            else if (equal(val, boundaryAbove))
                return boundaryAbove;

            if (roundType == RoundType::nearest) {
                // if we use "nearest" then we consider all values that are
                // insignificantly close to the halfway point to be on the
                // halfway point.
                double halfway = (boundaryAbove + boundaryBelow) / 2;

                if (equal(val, halfway)) {
                    // we are insignificantly close to the halfway point.
                    // boundaryAbove is closer to zero than boundaryBelow for
                    // negative number. We must return the one that is furthest
                    // away from zero
                    return (val < 0) ? boundaryBelow : boundaryAbove;
                } else
                    return std::round(val * pixelsPerDip) / pixelsPerDip;
            } else if (roundType == RoundType::up)
                return boundaryAbove;
            else
                return boundaryBelow;
        }

        /** Aligns the specified rectangle in DIP coordinates to physical pixel
           boundaries.

            positionRoundType indicates how the position is rounded,
           sizeRoundType indicates how the size is rounded.

            See pixelAlign(double, double, RoundType) for more information about
           how this alignment is done.

            */
        static Rect pixelAlign(const Rect &rect, double pixelsPerDip, RoundType positionRoundType,
                               RoundType sizeRoundType)
        {
            return Rect(pixelAlign(rect.x, pixelsPerDip, positionRoundType),
                        pixelAlign(rect.y, pixelsPerDip, positionRoundType),
                        pixelAlign(rect.width, pixelsPerDip, sizeRoundType),
                        pixelAlign(rect.height, pixelsPerDip, sizeRoundType));
        }

        /** Compares two DIP values while ignoring insignificant differences.
           Returns -1 if a is significantly smaller, 0 if the two values are not
           significantly different, 1 if a is significantly bigger.

            The function also works with infinity double values. The comparisons
           work as expected.

            If any of the parameters is the special NaN value then the return
           value is NaN
            */
        static double compare(double a, double b)
        {
            if (std::isfinite(a) && std::isfinite(b)) {
                double diff = a - b;
                if (diff < -significanceBoundary())
                    return -1;
                else if (diff > significanceBoundary())
                    return 1;
                else
                    return 0;
            } else {
                if (a < b)
                    return -1;
                else if (a > b)
                    return 1;
                else if (std::isnan(a) || std::isnan(b))
                    return std::numeric_limits<double>::quiet_NaN();
                else
                    return 0;
            }
        }

        /** Returns true if the specified two DIP values are "equal" in the
           sense that their differences are insignificant.*/
        static bool equal(double a, double b)
        {
            if (std::isfinite(a) && std::isfinite(b))
                return fabs(a - b) <= significanceBoundary();
            else {
                // when infinities or NaN are involved then we only need a
                // simple comparison
                return (a == b);
            }
        }

        /** Returns true if the specified Size objects with Dip values are
           "equal" in the sense that their differences are insignificant.*/
        static bool equal(const Size &a, const Size &b)
        {
            return (equal(a.width, b.width) && equal(a.height, b.height));
        }

        /** Returns true if the specified Point objects with Dip values are
           "equal" in the sense that their differences are insignificant.*/
        static bool equal(const Point &a, const Point &b) { return (equal(a.x, b.x) && equal(a.y, b.y)); }

        /** Returns true if the specified Rect objects with Dip values are
           "equal" in the sense that their differences are insignificant.*/
        static bool equal(const Rect &a, const Rect &b)
        {
            return (equal(a.x, b.x) && equal(a.y, b.y) && equal(a.width, b.width) && equal(a.height, b.height));
        }

        double getValue() const { return _value; }

        void operator=(double v) { _value = v; }

        operator double() const { return _value; }

        bool operator==(const Dip &o) const { return compare(_value, o._value) == 0; }

        bool operator!=(const Dip &o) const { return compare(_value, o._value) != 0; }

        bool operator<(const Dip &o) const { return compare(_value, o._value) < 0; }

        bool operator>(const Dip &o) const { return compare(_value, o._value) > 0; }

        bool operator<=(const Dip &o) const { return compare(_value, o._value) <= 0; }

        bool operator>=(const Dip &o) const { return compare(_value, o._value) >= 0; }

        bool operator==(double o) const { return compare(_value, o) == 0; }

        bool operator!=(double o) const { return compare(_value, o) != 0; }

        bool operator<(double o) const { return compare(_value, o) < 0; }

        bool operator>(double o) const { return compare(_value, o) > 0; }

        bool operator<=(double o) const { return compare(_value, o) <= 0; }

        bool operator>=(double o) const { return compare(_value, o) >= 0; }

        /** Returns the minimum difference between two DIP values that is
           considered significant.

            In the current implementation this is 0.0001, although you should
           not depend on that. The value might be changed in the future!

            Background information: why was this value chosen?
            --------------------------------------------------

            This value is used to iron out small differences introduced by the
           inaccuracies of floating point operations. The maximum error of a
           single floating point operation depends on how big the involved
           numbers are. Bigger numbers can introduce bigger errors. If you
           combine multiple operations then these errors can obviously get
           bigger and bigger.

            So we should set this boundary value as big as we can, but not so
           big that intentional differences between DIP values are ignored.

            To determine which differences are "insignificant" we have to look
           at what the value "1 DIP" represents. A DIP is roughly size of a
           "legacy pixel" on an old school 96 DPI display. 4k displays use
           pixels that are about 4 times smaller (i.e. each pixel is roughly
           0.25 DIPs).

            Since for 4k displays individual pixels are not visible to the human
           eye, differences below 0.1 DIP should be indiscernible on most normal
           displays. For huge displays or close viewing distances a lower limit
           might apply. Since we do not know the exact viewing conditions the
           limit was chosen to be way smaller than 0.1 to account for the
           unknown circumstances.

            */
        static constexpr double significanceBoundary()
        {
            // "double" floating point values have about a 15 decimal digit
            // precision. Desktop displays often have a width of 1000 - 2000
            // DIPs. If we allow for 1000 times bigger displays (=display walls)
            // then we will need about 7 digits before the decimal point.
            // I.e. we have 8 digits after the decimal point in that case.

            // So our chosen 0.0001 precision is well within the range where
            // floating point numbers in the expected ranges can be reasonably
            // accurate. The only differences can come from certain numbers not
            // being exactly representable as a floating point and those
            // differences should usually be less than 0.0001 in the given
            // number range - even if multiple operations are combined.

            return 0.0001;
        }

      private:
        double _value;
    };
}

inline bool operator==(double a, const bdn::Dip &b) { return b == a; }

inline bool operator!=(double a, const bdn::Dip &b) { return b != a; }

inline bool operator<(double a, const bdn::Dip &b) { return b > a; }

inline bool operator>(double a, const bdn::Dip &b) { return b < a; }

inline bool operator<=(double a, const bdn::Dip &b) { return b >= a; }

inline bool operator>=(double a, const bdn::Dip &b) { return b <= a; }
