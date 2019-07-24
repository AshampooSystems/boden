#include <bdn/Font.h>

namespace bdn
{
    Font Font::fromAny(std::any anyFont)
    {
        Font result;
        if (anyFont.type() == typeid(bdn::json)) {
            result = (Font)std::any_cast<bdn::json>(anyFont);
        } else if (anyFont.type() == typeid(Font)) {
            result = std::any_cast<Font>(anyFont);
        } else {
            throw std::bad_any_cast();
        }

        return result;
    }
}
