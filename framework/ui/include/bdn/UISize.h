#pragma once

#include <bdn/Size.h>
#include <bdn/UILength.h>

namespace bdn
{

    /** Represents a size within the context of a UI view.

        In contast to #Size, UISize specifies the width and height with UILength
        objects, thus allowing use of more abstract sizes that depend on the
       font size, etc.

        */
    struct UISize
    {
      public:
        UISize() {}

        UISize(const Size &size) : width(size.width), height(size.height) {}

        UISize(const UILength &width, const UILength &height) : width(width), height(height) {}

        UILength width;
        UILength height;
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const UISize &s)
    {
        return stream << "(" << s.width << " x " << s.height << ")";
    }
}

inline bool operator==(const bdn::UISize &a, const bdn::UISize &b)
{
    return (a.width == b.width && a.height == b.height);
}

inline bool operator!=(const bdn::UISize &a, const bdn::UISize &b) { return !operator==(a, b); }
