#pragma once

#include <bdn/Margin.h>

#include <cmath>
#include <limits>

namespace bdn
{
    struct Size
    {
      public:
        double width = 0;
        double height = 0;

      public:
        static constexpr double componentNone() { return std::numeric_limits<double>::infinity(); }
        static constexpr Size none() { return Size(componentNone(), componentNone()); }

      public:
        constexpr Size() = default;
        constexpr Size(double width, double height) : width(width), height(height) {}

      public:
        Size operator-(const Margin &margin) const { return Size(*this) -= margin; }
        Size &operator-=(const Margin &margin)
        {
            width -= margin.left + margin.right;
            height -= margin.top + margin.bottom;

            return *this;
        }

        Size operator+(const Margin &margin) const { return Size(*this) += margin; }
        Size &operator+=(const Margin &margin)
        {
            width += margin.left + margin.right;
            height += margin.top + margin.bottom;

            return *this;
        }

        Size operator+(const Size &o) const { return Size(*this) += o; }
        Size &operator+=(const Size &o)
        {
            width += o.width;
            height += o.height;

            return *this;
        }

        Size operator-(const Size &o) const { return Size(*this) -= o; }
        Size &operator-=(const Size &o)
        {
            width -= o.width;
            height -= o.height;

            return *this;
        }

        void applyMinimum(const Size &minSize)
        {
            if (std::isfinite(minSize.width) && (!std::isfinite(width) || width < minSize.width)) {
                width = minSize.width;
            }

            if (std::isfinite(minSize.height) && (!std::isfinite(height) || height < minSize.height)) {
                height = minSize.height;
            }
        }

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
