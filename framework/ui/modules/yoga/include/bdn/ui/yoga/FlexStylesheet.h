#pragma once

#include <bdn/String.h>
#include <bdn/ui/Json.h>
#include <optional>

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

        enum class Wrap
        {
            NoWrap,
            Wrap,
            WrapReverse
        };

        struct Edges
        {
            std::optional<float> all;
            std::optional<float> left;
            std::optional<float> top;
            std::optional<float> right;
            std::optional<float> bottom;

            bool operator==(const Edges &other) const
            {
                return all == other.all && left == other.left && right == other.right && top == other.top &&
                       bottom == other.bottom;
            }
        };

        struct Size
        {
            std::optional<float> width;
            std::optional<float> height;

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

        std::optional<float> flexBasis;
        std::optional<float> flexBasisPercent;

        float flexGrow = 0.0f;
        float flexShrink = 1.0;

        Edges padding;
        Edges margin;

        Size size;
        Size minimumSize;
        Size maximumSize;

        std::optional<float> aspectRatio;

        bool operator==(const FlexStylesheet &other) const
        {
            return flexDirection == other.flexDirection && layoutDirection == other.layoutDirection &&
                   flexWrap == other.flexWrap && alignSelf == other.alignSelf && alignItems == other.alignItems &&
                   alignContents == other.alignContents && justifyContent == other.justifyContent &&
                   flexBasis == other.flexBasis && flexBasisPercent == other.flexBasisPercent &&
                   flexGrow == other.flexGrow && flexShrink == other.flexShrink && padding == other.padding &&
                   margin == other.margin && size == other.size && minimumSize == other.minimumSize &&
                   maximumSize == other.maximumSize;
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

    template <> struct adl_serializer<bdn::ui::yoga::FlexStylesheet::Edges>
    {
        static void to_json(json &j, const bdn::ui::yoga::FlexStylesheet::Edges &edge)
        {
            j = {{"all", edge.all},
                 {"left", edge.left},
                 {"right", edge.right},
                 {"top", edge.top},
                 {"bottom", edge.bottom}};
        }

        static void from_json(const json &j, bdn::ui::yoga::FlexStylesheet::Edges &edge)
        {
            if (j.count("all") != 0) {
                edge.all = j.at("all").get<std::optional<float>>();
            }
            if (j.count("left") != 0) {
                edge.left = j.at("left").get<std::optional<float>>();
            }
            if (j.count("right") != 0) {
                edge.right = j.at("right").get<std::optional<float>>();
            }
            if (j.count("top") != 0) {
                edge.top = j.at("top").get<std::optional<float>>();
            }
            if (j.count("bottom") != 0) {
                edge.bottom = j.at("bottom").get<std::optional<float>>();
            }
        }
    };

    template <> struct adl_serializer<bdn::ui::yoga::FlexStylesheet::Size>
    {
        static void to_json(json &j, const bdn::ui::yoga::FlexStylesheet::Size &size)
        {
            j = {{"width", size.width}, {"height", size.height}};
        }

        static void from_json(const json &j, bdn::ui::yoga::FlexStylesheet::Size &size)
        {
            if (j.count("width") != 0) {
                size.width = j.at("width").get<std::optional<float>>();
            }
            if (j.count("height") != 0) {
                size.height = j.at("height").get<std::optional<float>>();
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

                {"flexBasis", sheet.flexBasis},
                {"flexBasisPercent", sheet.flexBasisPercent},

                {"flexGrow", sheet.flexGrow},
                {"flexShrink", sheet.flexShrink},

                {"aspectRatio", sheet.aspectRatio},

                {"margin", sheet.margin},
                {"padding", sheet.padding},

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
            if (j.count("flexBasis") != 0) {
                sheet.flexBasis = j.at("flexBasis").get<std::optional<float>>();
            }
            if (j.count("flexBasisPercent") != 0) {
                sheet.flexBasisPercent = j.at("flexBasisPercent").get<std::optional<float>>();
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
