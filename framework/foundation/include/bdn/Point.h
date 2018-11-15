#ifndef BDN_Point_H_
#define BDN_Point_H_

namespace bdn
{

    /** Represents a 2-D point (x, y)

        Point objects are supported by the global function bdn::toString().
        They can also be written to standard output streams (std::basic_ostream,
        bdn::TextOutStream, bdn::StringBuffer) with the << operator.
        */
    struct Point
    {
      public:
        double x = 0;
        double y = 0;

        Point() {}

        Point(double x, double y) : x(x), y(y) {}

        /** Adds the specified point to this point
            (by adding the coordinates of one point to those of the other).*/
        Point operator+(const Point &o) const { return Point(*this) += o; }

        /** Adds the specified size to this size.
            (by adding the coordinates of one point to those of the other).*/
        Point &operator+=(const Point &o)
        {
            x += o.x;
            y += o.y;

            return *this;
        }

        /** Subtracts the specified point from this point
            (by subtracting the coordinates of one point from those of the
           other).*/
        Point operator-(const Point &o) const { return Point(*this) -= o; }

        /** Subtracts the specified point from this point
            (by subtracting the coordinates of one point from those of the
           other).*/
        Point &operator-=(const Point &o)
        {
            x -= o.x;
            y -= o.y;

            return *this;
        }
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const Point &p)
    {
        return stream << "(" << p.x << ", " << p.y << ")";
    }
}

inline bool operator==(const bdn::Point &a, const bdn::Point &b) { return (a.x == b.x && a.y == b.y); }

inline bool operator!=(const bdn::Point &a, const bdn::Point &b) { return !operator==(a, b); }

/** Returns true if a's x and y are each smaller than b's */
inline bool operator<(const bdn::Point &a, const bdn::Point &b) { return (a.x < b.x && a.y < b.y); }

/** Returns true if a's x and y are each smaller or equal to b's */
inline bool operator<=(const bdn::Point &a, const bdn::Point &b) { return (a.x <= b.x && a.y <= b.y); }

/** Returns true if a's x and y are each bigger than b's */
inline bool operator>(const bdn::Point &a, const bdn::Point &b) { return (a.x > b.x && a.y > b.y); }

/** Returns true if a's x and y are each bigger or equal to b's */
inline bool operator>=(const bdn::Point &a, const bdn::Point &b) { return (a.x >= b.x && a.y >= b.y); }

#endif
