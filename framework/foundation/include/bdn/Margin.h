#pragma once

#include <ostream>

namespace bdn
{

    /** Represents the size of a margin or border.

        When margin components are listed individually (for example in a
       constructor) then their order is always the same as the order of the
       numbers on a clock: top, right, bottom, left

        This is also the same order that is used in the CSS standard.

        Margin objects are supported by the global function bdn::toString().
        They can also be written to standard output streams (std::basic_ostream,
        bdn::TextOutStream, bdn::StringBuffer) with the << operator.
        */
    struct Margin
    {
      public:
        double top = 0;
        double right = 0;
        double bottom = 0;
        double left = 0;

        Margin() = default;

        explicit Margin(double all)
        {
            top = all;
            right = all;
            bottom = all;
            left = all;
        }

        Margin(double topBottom, double leftRight)
        {
            top = topBottom;
            right = leftRight;
            bottom = topBottom;
            left = leftRight;
        }

        Margin(double top, double right, double bottom, double left)
        {
            this->top = top;
            this->right = right;
            this->bottom = bottom;
            this->left = left;
        }

        Margin operator+(const Margin &o) const { return Margin(*this) += o; }

        Margin operator-(const Margin &o) const { return Margin(*this) -= o; }

        Margin &operator+=(const Margin &o)
        {
            top += o.top;
            right += o.right;
            bottom += o.bottom;
            left += o.left;

            return *this;
        }

        Margin &operator-=(const Margin &o)
        {
            top -= o.top;
            right -= o.right;
            bottom -= o.bottom;
            left -= o.left;

            return *this;
        }
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const Margin &m)
    {
        return stream << "(" << m.top << ", " << m.right << ", " << m.bottom << ", " << m.left << ")";
    }
}

inline bool operator==(const bdn::Margin &a, const bdn::Margin &b)
{
    return (a.top == b.top && a.right == b.right && a.bottom == b.bottom && a.left == b.left);
}

inline bool operator!=(const bdn::Margin &a, const bdn::Margin &b) { return !operator==(a, b); }

/** Returns true if a's fields are each smaller than b's */
inline bool operator<(const bdn::Margin &a, const bdn::Margin &b)
{
    return (a.top < b.top && a.right < b.right && a.bottom < b.bottom && a.left < b.left);
}

/** Returns true if a's fields are each smaller or equal to b's */
inline bool operator<=(const bdn::Margin &a, const bdn::Margin &b)
{
    return (a.top <= b.top && a.right <= b.right && a.bottom <= b.bottom && a.left <= b.left);
}

/** Returns true if a's fields are each bigger than b's */
inline bool operator>(const bdn::Margin &a, const bdn::Margin &b)
{
    return (a.top > b.top && a.right > b.right && a.bottom > b.bottom && a.left > b.left);
}

/** Returns true if a's fields are each bigger or equal to b's */
inline bool operator>=(const bdn::Margin &a, const bdn::Margin &b)
{
    return (a.top >= b.top && a.right >= b.right && a.bottom >= b.bottom && a.left >= b.left);
}
