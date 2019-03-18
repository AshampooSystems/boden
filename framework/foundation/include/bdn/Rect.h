#pragma once

#include <bdn/Margin.h>
#include <bdn/Point.h>
#include <bdn/Size.h>

namespace bdn
{

    /** Represents the position and size of a simple two dimensional
        rectangle.

        Rect objects are supported by the global function bdn::toString().
        They can also be written to standard output streams (std::basic_ostream,
        bdn::TextOutStream, bdn::StringBuffer) with the << operator.

        */
    struct Rect
    {
      public:
        double x = 0;
        double y = 0;
        double width = 0;
        double height = 0;

        Rect() = default;

        Rect(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) {}

        Rect(const Point &pos, const Size &size) : x(pos.x), y(pos.y), width(size.width), height(size.height) {}

        Point position() const { return Point(x, y); }
        Size size() const { return Size(width, height); }

        /** Decrease the rect size by subtracting the specified margin.*/
        Rect operator-(const Margin &margin) const { return Rect(*this) -= margin; }

        /** Decrease the rect size by subtracting the specified margin.*/
        Rect &operator-=(const Margin &margin)
        {
            x += margin.left;
            y += margin.top;
            width -= margin.left + margin.right;
            height -= margin.top + margin.bottom;

            return *this;
        }

        /** Increase the rect size by adding the specified margin.*/
        Rect operator+(const Margin &margin) const { return Rect(*this) += margin; }

        /** Increase the rect size by adding the specified margin.*/
        Rect &operator+=(const Margin &margin)
        {
            x -= margin.left;
            y -= margin.top;
            width += margin.left + margin.right;
            height += margin.top + margin.bottom;

            return *this;
        }
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const Rect &r)
    {
        return stream << "(" << r.x << ", " << r.y << " | " << r.width << " x " << r.height << ")";
    }
}

inline bool operator==(const bdn::Rect &a, const bdn::Rect &b)
{
    return (a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height);
}

inline bool operator!=(const bdn::Rect &a, const bdn::Rect &b) { return !operator==(a, b); }
