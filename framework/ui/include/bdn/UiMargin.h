#pragma once

#include <bdn/UiLength.h>

namespace bdn
{

    /** Represents the size of a margin or border around a UI element.

        In contast to #Margin, UiMargin specifies the margin sizes with UiLength
        objects, thus allowing use of more abstract sizes that depend on the
       screen and system settings.

        */
    struct UiMargin
    {
      public:
        UiMargin() {}

        UiMargin(const UiLength &all) : top(all), right(all), bottom(all), left(all) {}

        UiMargin(const UiLength &topBottom, const UiLength &leftRight)
            : top(topBottom), right(leftRight), bottom(topBottom), left(leftRight)
        {}

        UiMargin(const UiLength &top, const UiLength &right, const UiLength &bottom, const UiLength &left)
            : top(top), right(right), bottom(bottom), left(left)
        {}

        bool operator==(const UiMargin &other) const
        {
            return top == other.top && bottom == other.bottom && left == other.left && right == other.right;
        }

        bool operator!=(const UiMargin &other) const { return !operator==(other); }

        UiLength top;
        UiLength right;
        UiLength bottom;
        UiLength left;
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const UiMargin &m)
    {
        return stream << "(" << m.top << ", " << m.right << ", " << m.bottom << ", " << m.left << ")";
    }
}
