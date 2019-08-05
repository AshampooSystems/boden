#pragma once

#include <bdn/AttributedString.h>
#include <string>
#include <variant>

#include <bdn/Json.h>

namespace bdn
{
    class Text : public std::variant<std::string, std::shared_ptr<AttributedString>>
    {
      public:
        enum class TruncateMode
        {
            Head,
            Tail,
            Middle,
            Clip,
            ClipWord
        };

      public:
        using variant<std::string, std::shared_ptr<AttributedString>>::variant;

        bool empty() const
        {
            return std::visit(
                [](auto &&arg) -> bool {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        return arg.empty();
                    } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                        return arg != nullptr;
                    }
                },
                *this);
        }

        operator std::string() const
        {
            return std::visit(
                [](auto &&arg) -> std::string {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
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
                    if constexpr (std::is_same_v<T, std::string>) {
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
                text = (std::string)j;
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
