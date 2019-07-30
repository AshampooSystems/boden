#pragma once

#include <bdn/AttributedString.h>
#include <bdn/String.h>
#include <variant>

#include <bdn/Json.h>

namespace bdn
{
    class Text : public std::variant<String, std::shared_ptr<AttributedString>>
    {
      public:
        using variant<String, std::shared_ptr<AttributedString>>::variant;

        bool empty() const
        {
            return std::visit(
                [](auto &&arg) -> bool {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, String>) {
                        return arg.empty();
                    } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                        return arg != nullptr;
                    }
                },
                *this);
        }

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

namespace nlohmann
{
    template <> struct adl_serializer<bdn::Text>
    {
        static void to_json(json &j, const bdn::Text &text)
        {
            std::visit(
                [&j](auto &&arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, bdn::String>) {
                        j = arg;
                    } else if constexpr (std::is_same_v<T, std::shared_ptr<bdn::AttributedString>>) {
                        j = arg->toJSON();
                    }
                },
                text);
        }

        static void from_json(const json &j, bdn::Text &text)
        {
            if (j.is_string()) {
                text = (bdn::String)j;
            } else if (j.is_object()) {
                auto attrString = std::make_shared<bdn::AttributedString>();
                attrString->fromJSON(j);
                text = attrString;
            }
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
