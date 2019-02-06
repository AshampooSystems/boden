#pragma once

#include <bdn/UILength.h>

namespace bdn
{

    /** Represents the size of a margin or border around a UI element.

        In contast to #Margin, UIMargin specifies the margin sizes with UILength
        objects, thus allowing use of more abstract sizes that depend on the
       screen and system settings.

        */
    struct UIMargin
    {
      public:
        UIMargin() {}

        UIMargin(const UILength &all) : top(all), right(all), bottom(all), left(all) {}

        UIMargin(const UILength &topBottom, const UILength &leftRight)
            : top(topBottom), right(leftRight), bottom(topBottom), left(leftRight)
        {}

        UIMargin(const UILength &top, const UILength &right, const UILength &bottom, const UILength &left)
            : top(top), right(right), bottom(bottom), left(left)
        {}

        bool operator==(const UIMargin &other) const
        {
            return top == other.top && bottom == other.bottom && left == other.left && right == other.right;
        }

        bool operator!=(const UIMargin &other) const { return !operator==(other); }

        UILength top;
        UILength right;
        UILength bottom;
        UILength left;
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const UIMargin &m)
    {
        return stream << "(" << m.top << ", " << m.right << ", " << m.bottom << ", " << m.left << ")";
    }
}
