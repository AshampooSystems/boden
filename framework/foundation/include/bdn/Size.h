#pragma once

#include <cmath>
#include <limits>
#include <ostream>

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
