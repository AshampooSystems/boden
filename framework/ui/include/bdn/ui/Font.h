#pragma once

#include <vector>

#include <bdn/Json.h>
#include <bdn/String.h>
#include <bdn/property/Property.h>

#ifdef JSON_THROW_USER
#define JSON_THROW JSON_THROW_USER
#elif defined(JSON_NOEXCEPTION)
#define JSON_THROW(exception) std::abort()
#else
#define JSON_THROW(exception) throw exception
#endif

namespace bdn::ui
{
    class Font
    {
      public:
        enum Weight
        {
            Inherit = -1,
            Normal = 400,
            Bold = 700
        };

        enum class Style
        {
            Inherit,
            Normal,
            Italic
        };

        enum class Variant
        {
            Inherit,
            Normal,
            SmallCaps
        };

        struct Size
        {
            enum class Type
            {
                Inherit = 0, // ?
                Medium = 1,
                Small = 2,
                XSmall = 3,
                XXSmall = 4,
                Large = 5,
                XLarge = 6,
                XXLarge = 7,
                Percent = 8,
                Points = 9,
                Pixels = 10,
            };

            Type type = Type::Inherit;
            float value = 11.0f;

            bool operator!=(const Size &other) const { return type != other.type || value != other.value; }
        };

      public:
        String family;
        Size size;
        Style style = Style::Inherit;
        Weight weight = Weight::Inherit;
        Variant variant = Variant::Inherit;

      public:
        bool operator!=(const Font &other) const
        {
            return family != other.family || size != other.size || style != other.style || weight != other.weight ||
                   variant != other.variant;
        }
    };
}

namespace nlohmann
{
    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::Font::Style, {
                                                           {bdn::ui::Font::Style::Inherit, "inherit"},
                                                           {bdn::ui::Font::Style::Normal, "normal"},
                                                           {bdn::ui::Font::Style::Italic, "italic"},
                                                       })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::Font::Variant, {
                                                             {bdn::ui::Font::Variant::Inherit, "inherit"},
                                                             {bdn::ui::Font::Variant::Normal, "normal"},
                                                             {bdn::ui::Font::Variant::SmallCaps, "smallcaps"},
                                                         })

    template <> struct adl_serializer<bdn::ui::Font::Size>
    {
        static void to_json(json &j, const bdn::ui::Font::Size &size)
        {
            switch (size.type) {
            case bdn::ui::Font::Size::Type::Inherit:
                j = {};
                return;
            case bdn::ui::Font::Size::Type::Medium:
                j = "Medium";
                break;
            case bdn::ui::Font::Size::Type::Small:
                j = "Small";
                break;
            case bdn::ui::Font::Size::Type::XSmall:
                j = "XSmall";
                break;
            case bdn::ui::Font::Size::Type::XXSmall:
                j = "XXSmall";
                break;
            case bdn::ui::Font::Size::Type::Large:
                j = "Large";
                break;
            case bdn::ui::Font::Size::Type::XLarge:
                j = "XLarge";
                break;
            case bdn::ui::Font::Size::Type::XXLarge:
                j = "XXLarge";
                break;
            case bdn::ui::Font::Size::Type::Percent:
                j = std::to_string(size.value) + "%";
                break;
            case bdn::ui::Font::Size::Type::Points:
                j = std::to_string(size.value) + " pt";
                break;
            case bdn::ui::Font::Size::Type::Pixels:
                j = size.value;
                break;
            }
        }

        static void from_json(const json &j, bdn::ui::Font::Size &size)
        {
            if (j.is_string()) {
                auto str = (bdn::String)j;

                std::istringstream sstream(str);
                sstream.imbue(std::locale("C"));

                float v;
                sstream >> v;

                if (sstream.fail()) {
                    std::transform(str.begin(), str.end(), str.begin(), &::tolower);
                    if (str == "medium") {
                        size.type = bdn::ui::Font::Size::Type::Medium;
                    } else if (str == "small") {
                        size.type = bdn::ui::Font::Size::Type::Small;
                    } else if (str == "xsmall") {
                        size.type = bdn::ui::Font::Size::Type::XSmall;
                    } else if (str == "xxsmall") {
                        size.type = bdn::ui::Font::Size::Type::XXSmall;
                    } else if (str == "large") {
                        size.type = bdn::ui::Font::Size::Type::Large;
                    } else if (str == "xlarge") {
                        size.type = bdn::ui::Font::Size::Type::XLarge;
                    } else if (str == "xxlarge") {
                        size.type = bdn::ui::Font::Size::Type::XXLarge;
                    } else {
                        JSON_THROW(nlohmann::json::other_error::create(501, "Failed parsing: \"" + str + "\""));
                    }
                } else {
                    bdn::String unit;
                    sstream >> unit;
                    if (sstream.fail() || unit == "px") {
                        size.value = v;
                        size.type = bdn::ui::Font::Size::Type::Pixels;
                    } else if (unit == "%") {
                        size.value = v;
                        size.type = bdn::ui::Font::Size::Type::Percent;
                    } else if (unit == "pt") {
                        size.value = v;
                        size.type = bdn::ui::Font::Size::Type::Points;
                    } else {
                        JSON_THROW(nlohmann::json::other_error::create(501, "Invalid unit: \"" + unit + "\""));
                    }
                }
            }
        }
    };

    template <> struct adl_serializer<bdn::ui::Font::Weight>
    {
        inline static const std::map<bdn::String, int> nameToWeight{
            {"thin", 100},      {"extra light", 200}, {"light", 300},      {"normal", 400}, {"medium", 500},
            {"semi bold", 600}, {"bold", 700},        {"extra bold", 800}, {"black", 900}};

        static void to_json(json &j, const bdn::ui::Font::Weight &weight)
        {
            int w = weight;

            if (w % 100 == 0) {
                auto it = std::find_if(nameToWeight.begin(), nameToWeight.end(), [w](auto p) { return p.second == w; });
                if (it != nameToWeight.end()) {
                    j = it->first;
                } else {
                    j = w;
                }
            }
        }

        static void from_json(const json &j, bdn::ui::Font::Weight &weight)
        {
            if (j.is_number()) {
                weight = (bdn::ui::Font::Weight)((int)j);
            } else if (j.is_string()) {
                auto str = (bdn::String)j;
                std::transform(str.begin(), str.end(), str.begin(), &::tolower);

                auto it = nameToWeight.find(str);
                if (it != nameToWeight.end()) {
                    weight = (bdn::ui::Font::Weight)it->second;
                } else {
                    JSON_THROW(
                        nlohmann::json::other_error::create(501, "Failed parsing weight: \"" + (std::string)j + "\""));
                }
            }
        }
    };

    template <> struct adl_serializer<bdn::ui::Font>
    {
        static void to_json(json &j, const bdn::ui::Font &font)
        {
            j = {{"family", font.family},
                 {"size", font.size},
                 {"weight", font.weight},
                 {"style", font.style},
                 {"variant", font.variant}};

            // j = {color.red(), color.green(), color.blue(), color.alpha()};
        }

        static void from_json(const json &j, bdn::ui::Font &font)
        {
            if (j.count("family")) {
                font.family = j.at("family");
            }
            if (j.count("size")) {
                font.size = j.at("size");
            }
            if (j.count("weight")) {
                font.weight = j.at("weight");
            }
            if (j.count("style")) {
                font.style = j.at("style");
            }
            if (j.count("variant")) {
                font.variant = j.at("variant");
            }
        }
    };
}
