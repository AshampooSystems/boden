#pragma once

#include <bdn/AttributedString.h>
#include <bdn/String.h>
#include <variant>

namespace bdn
{
    class Text : public std::variant<String, std::shared_ptr<AttributedString>>
    {
      public:
        using variant<String, std::shared_ptr<AttributedString>>::variant;

        operator String() const
        {
            return std::visit(
                [](auto &&arg) -> String {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, String>) {
                        return arg;
                    } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                        return "";
                    }
                },
                *this);
        }
    };
}

inline bool operator!=(const bdn::Text &lhs, const bdn::Text &rhs)
{
    if (lhs.index() != rhs.index()) {
        return true;
    }

    if (lhs.index() == 0) {
        return std::get<0>(lhs) != std::get<0>(rhs);
    }
    if (lhs.index() == 1) {
        return std::get<1>(lhs) != std::get<1>(rhs);
    }

    return false;
}
