#pragma once

#include <bdn/Margin.h>

#include <cmath>
#include <limits>

namespace bdn
{

    /** Represents the a size (width and height).

        Size objects are supported by the global function bdn::toString().
        They can also be written to standard output streams (std::basic_ostream,
        bdn::TextOutStream, bdn::StringBuffer) with the << operator.

        */
    struct Size
    {
      public:
        double width = 0;
        double height = 0;

      public:
        /** Returns a number that can be used as a Size component (width and/or
           height) to indicate a value that is intentionally left unspecified.

            Arithmetic operations where one side is componentNone always result
           in componentNone.

            componentNone is often used to indicate "unlimited" and similar
           special circumstances (for example when size limits are represented
           with bdn::Size object.

            The value returned by componentNone() can be used in normal equality
           comparisons, i.e. it does *not* behave like the NaN floating point
           value).

            The actual value of componentNone is the positive inifinity floating
           point value.

            Also see none().
            */
        static constexpr double componentNone() { return std::numeric_limits<double>::infinity(); }

        /** Returns a Size object in which both width and height are
           intentionally left unspecified (they both have the value
           componentNone() ).

            Arithmetic operations where one side is none always result in none.

            none is often used to indicate "unlimited" and similar special
           circumstances (for example when size limits are represented with
           bdn::Size object.

            Also see componentNone().
            */
        static constexpr Size none() { return Size(componentNone(), componentNone()); }

        constexpr Size() = default;

        constexpr Size(double width, double height) : width(width), height(height) {}

        /** Subtracts the size of the specified margin from the size.*/
        Size operator-(const Margin &margin) const { return Size(*this) -= margin; }

        /** Subtracts the size of the specified margin from the size.*/
        Size &operator-=(const Margin &margin)
        {
            width -= margin.left + margin.right;
            height -= margin.top + margin.bottom;

            return *this;
        }

        /** Adds the size of the specified margin to the size.*/
        Size operator+(const Margin &margin) const { return Size(*this) += margin; }

        /** Adds the size of the specified margin to the size.*/
        Size &operator+=(const Margin &margin)
        {
            width += margin.left + margin.right;
            height += margin.top + margin.bottom;

            return *this;
        }

        /** Adds the specified size to this size.*/
        Size operator+(const Size &o) const { return Size(*this) += o; }

        /** Adds the specified size to this size.*/
        Size &operator+=(const Size &o)
        {
            width += o.width;
            height += o.height;

            return *this;
        }

        /** Subtracts the specified size from this size.*/
        Size operator-(const Size &o) const { return Size(*this) -= o; }

        /** Subtracts the specified size from this size.*/
        Size &operator-=(const Size &o)
        {
            width -= o.width;
            height -= o.height;

            return *this;
        }

        /** Applies a minimum size constraint to the Size object.

            minSize is a Size object that defines the constraint.

            width and/or height of minSize can have the special value inifity,
           NaN or Size::componentNone() to indicate that this component should
           not be constrained. That also means that if you pass Size::none() to
           the function then the function has no effect.

            If a minSize component has a normal finite value then that
           represents the desired minimum value for that component. If the
           corresponding component of the Size object is below that then it is
           set to the minimum value.

            */
        void applyMinimum(const Size &minSize)
        {
            if (std::isfinite(minSize.width) && (!std::isfinite(width) || width < minSize.width)) {
                width = minSize.width;
            }

            if (std::isfinite(minSize.height) && (!std::isfinite(height) || height < minSize.height)) {
                height = minSize.height;
            }
        }

        /** Applies a maximum size constraint to the Size object.

            maxSize is a Size object that defines the constraint.

            width and/or height of maxSize can have the special value inifity,
           NaN or Size::componentNone() to indicate that this component should
           not be constrained. That also means that if you pass Size::none() to
           the function then the function has no effect.

            If a maxSize component has a normal finite value then that
           represents the desired maximum value for that component. If the
           corresponding component of the Size object is above that then it is
           set to the maximum value.

            */
        void applyMaximum(const Size &maxSize)
        {
            if (std::isfinite(maxSize.width) && (!std::isfinite(width) || width > maxSize.width)) {
                width = maxSize.width;
            }

            if (std::isfinite(maxSize.height) && (!std::isfinite(height) || height > maxSize.height)) {
                height = maxSize.height;
            }
        }
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const Size &s)
    {
        return stream << "(" << s.width << " x " << s.height << ")";
    }
}

inline bool operator==(const bdn::Size &a, const bdn::Size &b) { return (a.width == b.width && a.height == b.height); }

inline bool operator!=(const bdn::Size &a, const bdn::Size &b) { return !operator==(a, b); }

/** Returns true if a's width and height are each smaller than b's */
inline bool operator<(const bdn::Size &a, const bdn::Size &b) { return (a.width < b.width && a.height < b.height); }

/** Returns true if a's width and height are each smaller or equal to b's */
inline bool operator<=(const bdn::Size &a, const bdn::Size &b) { return (a.width <= b.width && a.height <= b.height); }

/** Returns true if a's width and height are each bigger than b's */
inline bool operator>(const bdn::Size &a, const bdn::Size &b) { return (a.width > b.width && a.height > b.height); }

/** Returns true if a's width and height are each bigger or equal to b's */
inline bool operator>=(const bdn::Size &a, const bdn::Size &b) { return (a.width >= b.width && a.height >= b.height); }
