#pragma once

#include <bdn/String.h>
#include <bdn/ui/Json.h>
#include <optional>
#include <variant>

#ifdef JSON_THROW_USER
#define JSON_THROW JSON_THROW_USER
#elif defined(JSON_NOEXCEPTION)
#define JSON_THROW(exception) std::abort()
#else
#define JSON_THROW(exception) throw exception
#endif

namespace bdn::ui::yoga
{
    class FlexStylesheet
    {
      public:
        enum class Direction
        {
            Column,
            ColumnReverse,
            Row,
            RowReverse
        };

        enum class LayoutDirection
        {
            Inherit,
            LTR,
            RTL
        };

        enum class Align
        {
            Auto,
            FlexStart,
            Center,
            FlexEnd,
            Stretch,
            Baseline,
            SpaceBetween,
            SpaceAround
        };

        enum class Justify
        {
            FlexStart,
            Center,
            FlexEnd,
            SpaceBetween,
            SpaceAround,
            SpaceEvenly
        };

        enum class PositionType
        {
            Relative,
            Absolute
        };

        enum class Wrap
        {
            NoWrap,
            Wrap,
            WrapReverse
        };

        struct ValueWithUnit
        {
            enum class Unit
            {
                PIXEL,
                PERCENT
            } unit = Unit::PIXEL;
            float value = NAN;

            ValueWithUnit() : unit(Unit::PIXEL), value(NAN) {}

            constexpr bool isPixel() const { return unit == Unit::PIXEL; }
            constexpr bool isPercent() const { return unit == Unit::PERCENT; }

            bool operator==(const ValueWithUnit &other) const { return value == other.value && unit == other.unit; }
        };

        struct Edges
        {
            std::optional<ValueWithUnit> all;
            std::optional<ValueWithUnit> left;
            std::optional<ValueWithUnit> top;
            std::optional<ValueWithUnit> right;
            std::optional<ValueWithUnit> bottom;

            bool operator==(const Edges &other) const
            {
                return all == other.all && left == other.left && right == other.right && top == other.top &&
                       bottom == other.bottom;
            }
        };

        struct Size
        {
            std::optional<ValueWithUnit> width;
            std::optional<ValueWithUnit> height;

            bool operator==(const Size &other) const { return width == other.width && height == other.height; }
        };

      public:
        Direction flexDirection = Direction::Column;
        LayoutDirection layoutDirection = LayoutDirection::Inherit;

        Wrap flexWrap = Wrap::NoWrap;

        Align alignSelf = Align::Auto;
        Align alignItems = Align::Stretch;
        Align alignContents = Align::Stretch;

        Justify justifyContent = Justify::FlexStart;

        std::optional<ValueWithUnit> flexBasis;

        float flexGrow = 0.0f;
        float flexShrink = 1.0;

        Edges padding;
        Edges margin;
        Edges position;

        PositionType positionType = PositionType::Relative;

        Size size;
        Size minimumSize;
        Size maximumSize;

        std::optional<float> aspectRatio;

        bool operator==(const FlexStylesheet &other) const
        {
            return flexDirection == other.flexDirection && layoutDirection == other.layoutDirection &&
                   flexWrap == other.flexWrap && alignSelf == other.alignSelf && alignItems == other.alignItems &&
                   alignContents == other.alignContents && justifyContent == other.justifyContent &&
                   flexBasis == other.flexBasis && flexGrow == other.flexGrow && flexShrink == other.flexShrink &&
                   padding == other.padding && margin == other.margin && size == other.size &&
                   minimumSize == other.minimumSize && maximumSize == other.maximumSize && position == other.position &&
                   positionType != other.positionType;
        }
    };
}

namespace nlohmann
{
    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::yoga::FlexStylesheet::Direction,
                                 {
                                     {bdn::ui::yoga::FlexStylesheet::Direction::Column, "Column"},
                                     {bdn::ui::yoga::FlexStylesheet::Direction::ColumnReverse, "ColumnReverse"},
                                     {bdn::ui::yoga::FlexStylesheet::Direction::Row, "Row"},
                                     {bdn::ui::yoga::FlexStylesheet::Direction::RowReverse, "RowReverse"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::yoga::FlexStylesheet::Align,
                                 {
                                     {bdn::ui::yoga::FlexStylesheet::Align::Auto, "Auto"},
                                     {bdn::ui::yoga::FlexStylesheet::Align::FlexStart, "FlexStart"},
                                     {bdn::ui::yoga::FlexStylesheet::Align::Center, "Center"},
                                     {bdn::ui::yoga::FlexStylesheet::Align::FlexEnd, "FlexEnd"},
                                     {bdn::ui::yoga::FlexStylesheet::Align::Stretch, "Stretch"},
                                     {bdn::ui::yoga::FlexStylesheet::Align::Baseline, "Baseline"},
                                     {bdn::ui::yoga::FlexStylesheet::Align::SpaceBetween, "SpaceBetween"},
                                     {bdn::ui::yoga::FlexStylesheet::Align::SpaceAround, "SpaceAround"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::yoga::FlexStylesheet::LayoutDirection,
                                 {
                                     {bdn::ui::yoga::FlexStylesheet::LayoutDirection::Inherit, "Inherit"},
                                     {bdn::ui::yoga::FlexStylesheet::LayoutDirection::LTR, "LTR"},
                                     {bdn::ui::yoga::FlexStylesheet::LayoutDirection::RTL, "RTL"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::yoga::FlexStylesheet::Justify,
                                 {
                                     {bdn::ui::yoga::FlexStylesheet::Justify::FlexStart, "FlexStart"},
                                     {bdn::ui::yoga::FlexStylesheet::Justify::Center, "Center"},
                                     {bdn::ui::yoga::FlexStylesheet::Justify::FlexEnd, "FlexEnd"},
                                     {bdn::ui::yoga::FlexStylesheet::Justify::SpaceBetween, "SpaceBetween"},
                                     {bdn::ui::yoga::FlexStylesheet::Justify::SpaceAround, "SpaceAround"},
                                     {bdn::ui::yoga::FlexStylesheet::Justify::SpaceEvenly, "SpaceEvenly"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::yoga::FlexStylesheet::PositionType,
                                 {{bdn::ui::yoga::FlexStylesheet::PositionType::Relative, "Relative"},
                                  {bdn::ui::yoga::FlexStylesheet::PositionType::Absolute, "Absolute"}})

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::ui::yoga::FlexStylesheet::Wrap,
                                 {
                                     {bdn::ui::yoga::FlexStylesheet::Wrap::NoWrap, "NoWrap"},
                                     {bdn::ui::yoga::FlexStylesheet::Wrap::Wrap, "Wrap"},
                                     {bdn::ui::yoga::FlexStylesheet::Wrap::WrapReverse, "WrapReverse"},
                                 })

    template <typename T> struct adl_serializer<std::optional<T>>
    {
        static void to_json(json &j, const std::optional<T> &opt)
        {
            if (opt) {
                j = *opt;
            } else {
                j = nullptr;
            }
        }

        static void from_json(const json &j, std::optional<T> &opt)
        {
            if (j.is_null()) {
                opt = std::nullopt;
            } else {
                opt = j.get<T>();
            }
        }
    };

    template <> struct adl_serializer<bdn::ui::yoga::FlexStylesheet::ValueWithUnit>
    {
        using ValueWithUnit = bdn::ui::yoga::FlexStylesheet::ValueWithUnit;
        static void to_json(json &j, const ValueWithUnit value)
        {
            if (value.unit == ValueWithUnit::Unit::PIXEL) {
                j = value.value;
            } else {
                j = std::to_string(value.value) + "%";
            }
        }

        static void from_json(const json &j, ValueWithUnit &value)
        {
            if (j.is_number()) {
                value.value = j.get<float>();
                value.unit = ValueWithUnit::Unit::PIXEL;
            } else if (j.is_string()) {
                std::string s = j.get<std::string>();
                std::string unit;

                std::istringstream istr(s);
                istr.imbue(std::locale("C"));
                istr >> value.value;

                if (istr.fail()) {
                    JSON_THROW(
                        nlohmann::json::other_error::create(501, "Failed parsing string to number: \"" + s + "\""));
                }

                istr >> unit;

                if (unit == "%") {
                    value.unit = ValueWithUnit::Unit::PERCENT;
                } else if (unit == "px" || unit == "") {
                    value.unit = ValueWithUnit::Unit::PIXEL;
                } else {
                    JSON_THROW(
                        nlohmann::json::other_error::create(501, "Invalid unit: \"" + unit + "\" in: \"" + s + "\""));
                }
            } else {
                JSON_THROW(nlohmann::json::type_error::create(302, "type must be number or string, but is " +
                                                                       std::string(j.type_name())));
            }
        }
    };

    template <> struct adl_serializer<bdn::ui::yoga::FlexStylesheet::Edges>
    {
        using FlexStylesheet = bdn::ui::yoga::FlexStylesheet;

        static void to_json(json &j, const FlexStylesheet::Edges &edge)
        {
            j = {{"all", edge.all},
                 {"left", edge.left},
                 {"right", edge.right},
                 {"top", edge.top},
                 {"bottom", edge.bottom}};
        }

        static void from_json(const json &j, FlexStylesheet::Edges &edge)
        {
            if (j.is_object()) {
                if (j.count("all") != 0) {
                    edge.all = j.at("all").get<std::optional<FlexStylesheet::ValueWithUnit>>();
                }
                if (j.count("left") != 0) {
                    edge.left = j.at("left").get<std::optional<FlexStylesheet::ValueWithUnit>>();
                }
                if (j.count("right") != 0) {
                    edge.right = j.at("right").get<std::optional<FlexStylesheet::ValueWithUnit>>();
                }
                if (j.count("top") != 0) {
                    edge.top = j.at("top").get<std::optional<FlexStylesheet::ValueWithUnit>>();
                }
                if (j.count("bottom") != 0) {
                    edge.bottom = j.at("bottom").get<std::optional<FlexStylesheet::ValueWithUnit>>();
                }
            } else if (j.is_number() || j.is_string()) {
                edge.all = j.get<std::optional<FlexStylesheet::ValueWithUnit>>();
            }
        }
    };

    template <> struct adl_serializer<bdn::ui::yoga::FlexStylesheet::Size>
    {
        using FlexStylesheet = bdn::ui::yoga::FlexStylesheet;

        static void to_json(json &j, const FlexStylesheet::Size &size)
        {
            j = {{"width", size.width}, {"height", size.height}};
        }

        static void from_json(const json &j, FlexStylesheet::Size &size)
        {
            if (j.count("width") != 0) {
                size.width = j.at("width").get<std::optional<FlexStylesheet::ValueWithUnit>>();
            }
            if (j.count("height") != 0) {
                size.height = j.at("height").get<std::optional<FlexStylesheet::ValueWithUnit>>();
            }
        }
    };

    template <> struct adl_serializer<bdn::ui::yoga::FlexStylesheet>
    {
        static void to_json(json &j, const bdn::ui::yoga::FlexStylesheet &sheet)
        {
            j = {
                {"direction", sheet.flexDirection},
                {"layoutDirection", sheet.layoutDirection},
                {"flexWrap", sheet.flexWrap},
                {"alignSelf", sheet.alignSelf},
                {"alignItems", sheet.alignItems},
                {"alignContents", sheet.alignContents},
                {"justifyContent", sheet.justifyContent},
                {"positionType", sheet.positionType},

                {"flexBasis", sheet.flexBasis},

                {"flexGrow", sheet.flexGrow},
                {"flexShrink", sheet.flexShrink},

                {"aspectRatio", sheet.aspectRatio},

                {"margin", sheet.margin},
                {"padding", sheet.padding},
                {"position", sheet.position},

                {"size", sheet.size},
                {"maximumSize", sheet.maximumSize},
                {"minimumSize", sheet.minimumSize},
            };
        }

        static void from_json(const json &j, bdn::ui::yoga::FlexStylesheet &sheet)
        {
            if (j.count("direction") != 0) {
                sheet.flexDirection = j.at("direction").get<bdn::ui::yoga::FlexStylesheet::Direction>();
            }
            if (j.count("layoutDirection") != 0) {
                sheet.layoutDirection = j.at("layoutDirection");
            }
            if (j.count("flexWrap") != 0) {
                sheet.flexWrap = j.at("flexWrap");
            }
            if (j.count("alignSelf") != 0) {
                sheet.alignSelf = j.at("alignSelf");
            }
            if (j.count("alignItems") != 0) {
                sheet.alignItems = j.at("alignItems");
            }
            if (j.count("alignContents") != 0) {
                sheet.alignContents = j.at("alignContents");
            }
            if (j.count("justifyContent") != 0) {
                sheet.justifyContent = j.at("justifyContent");
            }
            if (j.count("positionType") != 0) {
                sheet.positionType = j.at("positionType");
            }
            if (j.count("flexBasis") != 0) {
                sheet.flexBasis = j.at("flexBasis").get<std::optional<bdn::ui::yoga::FlexStylesheet::ValueWithUnit>>();
            }
            if (j.count("flexGrow") != 0) {
                sheet.flexGrow = j.at("flexGrow");
            }
            if (j.count("flexShrink") != 0) {
                sheet.flexShrink = j.at("flexShrink");
            }
            if (j.count("aspectRatio") != 0) {
                sheet.aspectRatio = j.at("aspectRatio").get<std::optional<float>>();
            }
            if (j.count("margin") != 0) {
                sheet.margin = j.at("margin");
            }
            if (j.count("padding") != 0) {
                sheet.padding = j.at("padding");
            }
            if (j.count("position") != 0) {
                sheet.position = j.at("position");
            }
            if (j.count("size") != 0) {
                sheet.size = j.at("size");
            }
            if (j.count("maximumSize") != 0) {
                sheet.maximumSize = j.at("maximumSize");
            }
            if (j.count("minimumSize") != 0) {
                sheet.minimumSize = j.at("minimumSize");
            }
        }
    };
}

#define FlexJsonStringify(str...) JsonStringify({"flex" : str})
