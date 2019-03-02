#pragma once

#include <bdn/Layout.h>
#include <optional>

namespace bdn
{
    class FlexStylesheet : public LayoutStylesheet
    {
      public:
        const char *type{"flex-layout"};

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

        template <class Manip> FlexStylesheet &operator<<(const Manip &manipulator)
        {
            manipulator.apply(this);
            return *this;
        }

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

#define DEF_MANIPULATOR_OPERATORS()                                                                                    \
    template <class Manipulator> FlexStylesheet operator<<(const Manipulator &manipulator)                             \
    {                                                                                                                  \
        FlexStylesheet sheet;                                                                                          \
        sheet << *this << manipulator;                                                                                 \
        return sheet;                                                                                                  \
    }                                                                                                                  \
    operator FlexStylesheet()                                                                                          \
    {                                                                                                                  \
        FlexStylesheet sheet;                                                                                          \
        return sheet << *this;                                                                                         \
    }

#define DEF_MANIPULATOR(Type, Name, member)                                                                            \
    struct Flex##Name                                                                                                  \
    {                                                                                                                  \
        Flex##Name(Type v) : _v(v) {}                                                                                  \
        void apply(FlexStylesheet *s) const { s->member = _v; }                                                        \
        DEF_MANIPULATOR_OPERATORS()                                                                                    \
        Type _v;                                                                                                       \
    };

#define DEF_ALL_SIZE_MANIPULATOR(Type, Name, member)                                                                   \
    struct Flex##Name                                                                                                  \
    {                                                                                                                  \
        Flex##Name(Type width, Type height) : _w(width), _h(height) {}                                                 \
        void apply(FlexStylesheet *s) const                                                                            \
        {                                                                                                              \
            s->member.width = _w;                                                                                      \
            s->member.height = _h;                                                                                     \
        }                                                                                                              \
        DEF_MANIPULATOR_OPERATORS()                                                                                    \
        Type _w, _h;                                                                                                   \
    };

#define DEF_ALL_EDGE_MANIPULATOR(Type, Name, member)                                                                   \
    struct Flex##Name                                                                                                  \
    {                                                                                                                  \
        Flex##Name(Type left, Type right, Type top, Type bottom) : _l(left), _r(right), _t(top), _b(bottom) {}         \
        void apply(FlexStylesheet *s) const                                                                            \
        {                                                                                                              \
            s->member.left = _l;                                                                                       \
            s->member.right = _r;                                                                                      \
            s->member.top = _t;                                                                                        \
            s->member.bottom = _t;                                                                                     \
        }                                                                                                              \
        DEF_MANIPULATOR_OPERATORS()                                                                                    \
        Type _l, _r, _t, _b;                                                                                           \
    };

#define DEF_EDGE_MANIPULATOR(Name, member)                                                                             \
    DEF_MANIPULATOR(std::optional<float>, Name##All, member.all)                                                       \
    DEF_MANIPULATOR(std::optional<float>, Name##Left, member.left)                                                     \
    DEF_MANIPULATOR(std::optional<float>, Name##Right, member.right)                                                   \
    DEF_MANIPULATOR(std::optional<float>, Name##Top, member.top)                                                       \
    DEF_MANIPULATOR(std::optional<float>, Name##Bottom, member.bottom)                                                 \
    DEF_ALL_EDGE_MANIPULATOR(std::optional<float>, Name, member)

#define DEF_SIZE_MANIPULATOR(Name, member)                                                                             \
    DEF_MANIPULATOR(std::optional<float>, Name##Width, member.width)                                                   \
    DEF_MANIPULATOR(std::optional<float>, Name##Height, member.height)                                                 \
    DEF_ALL_SIZE_MANIPULATOR(std::optional<float>, Name, member)

    struct Flex
    {
        void apply(FlexStylesheet *) const {}
        DEF_MANIPULATOR_OPERATORS()
    };

    DEF_MANIPULATOR(FlexStylesheet::Direction, Direction, flexDirection)
    DEF_MANIPULATOR(FlexStylesheet::LayoutDirection, LayoutDirection, layoutDirection)

    DEF_MANIPULATOR(float, Grow, flexGrow)
    DEF_MANIPULATOR(float, Shrink, flexShrink)

    DEF_MANIPULATOR(std::optional<float>, Basis, flexBasis)
    DEF_MANIPULATOR(std::optional<float>, BasisPercent, flexBasisPercent)

    DEF_MANIPULATOR(FlexStylesheet::Wrap, Wrap, flexWrap)
    DEF_MANIPULATOR(FlexStylesheet::Align, AlignSelf, alignSelf)
    DEF_MANIPULATOR(FlexStylesheet::Align, AlignItems, alignItems)
    DEF_MANIPULATOR(FlexStylesheet::Align, AlignContents, alignContents)
    DEF_MANIPULATOR(FlexStylesheet::Justify, JustifyContent, justifyContent)

    DEF_EDGE_MANIPULATOR(Padding, padding)
    DEF_EDGE_MANIPULATOR(Margin, margin)

    DEF_SIZE_MANIPULATOR(Size, size)
    DEF_SIZE_MANIPULATOR(MaximumSize, maximumSize)
    DEF_SIZE_MANIPULATOR(MinimumSize, minimumSize)

    DEF_MANIPULATOR(std::optional<float>, AspectRatio, aspectRatio)
}

#ifdef BDN_HAS_NLOHMANN_JSON

#include <nlohmann/json.hpp>

namespace nlohmann
{
    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::FlexStylesheet::Direction,
                                 {
                                     {bdn::FlexStylesheet::Direction::Column, "Column"},
                                     {bdn::FlexStylesheet::Direction::ColumnReverse, "ColumnReverse"},
                                     {bdn::FlexStylesheet::Direction::Row, "Row"},
                                     {bdn::FlexStylesheet::Direction::RowReverse, "RowReverse"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::FlexStylesheet::Align,
                                 {
                                     {bdn::FlexStylesheet::Align::Auto, "Auto"},
                                     {bdn::FlexStylesheet::Align::FlexStart, "FlexStart"},
                                     {bdn::FlexStylesheet::Align::Center, "Center"},
                                     {bdn::FlexStylesheet::Align::FlexEnd, "FlexEnd"},
                                     {bdn::FlexStylesheet::Align::Stretch, "Stretch"},
                                     {bdn::FlexStylesheet::Align::Baseline, "Baseline"},
                                     {bdn::FlexStylesheet::Align::SpaceBetween, "SpaceBetween"},
                                     {bdn::FlexStylesheet::Align::SpaceAround, "SpaceAround"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::FlexStylesheet::LayoutDirection,
                                 {
                                     {bdn::FlexStylesheet::LayoutDirection::Inherit, "Inherit"},
                                     {bdn::FlexStylesheet::LayoutDirection::LTR, "LTR"},
                                     {bdn::FlexStylesheet::LayoutDirection::RTL, "RTL"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::FlexStylesheet::Justify,
                                 {
                                     {bdn::FlexStylesheet::Justify::FlexStart, "FlexStart"},
                                     {bdn::FlexStylesheet::Justify::Center, "Center"},
                                     {bdn::FlexStylesheet::Justify::FlexEnd, "FlexEnd"},
                                     {bdn::FlexStylesheet::Justify::SpaceBetween, "SpaceBetween"},
                                     {bdn::FlexStylesheet::Justify::SpaceAround, "SpaceAround"},
                                     {bdn::FlexStylesheet::Justify::SpaceEvenly, "SpaceEvenly"},
                                 })

    NLOHMANN_JSON_SERIALIZE_ENUM(bdn::FlexStylesheet::Wrap, {
                                                                {bdn::FlexStylesheet::Wrap::NoWrap, "NoWrap"},
                                                                {bdn::FlexStylesheet::Wrap::Wrap, "Wrap"},
                                                                {bdn::FlexStylesheet::Wrap::WrapReverse, "WrapReverse"},
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
#define ALLOW_NON_EXISTING(x)                                                                                          \
    try {                                                                                                              \
        x                                                                                                              \
    }                                                                                                                  \
    catch (const json::out_of_range &) {                                                                               \
    }

    template <> struct adl_serializer<bdn::FlexStylesheet::Edges>
    {
        static void to_json(json &j, const bdn::FlexStylesheet::Edges &edge)
        {
            j = {{"all", edge.all},
                 {"left", edge.left},
                 {"right", edge.right},
                 {"top", edge.top},
                 {"bottom", edge.bottom}};
        }

        static void from_json(const json &j, bdn::FlexStylesheet::Edges &edge)
        {
            ALLOW_NON_EXISTING(edge.all = j.at("all").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(edge.left = j.at("left").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(edge.right = j.at("right").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(edge.top = j.at("top").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(edge.bottom = j.at("bottom").get<std::optional<float>>();)
        }
    };

    template <> struct adl_serializer<bdn::FlexStylesheet::Size>
    {
        static void to_json(json &j, const bdn::FlexStylesheet::Size &size)
        {
            j = {{"width", size.width}, {"height", size.height}};
        }

        static void from_json(const json &j, bdn::FlexStylesheet::Size &size)
        {
            ALLOW_NON_EXISTING(size.width = j.at("width").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(size.height = j.at("height").get<std::optional<float>>();)
        }
    };

    template <> struct adl_serializer<bdn::FlexStylesheet>
    {
        static void to_json(json &j, const bdn::FlexStylesheet &sheet)
        {
            j = {
                {"direction", sheet.flexDirection},
                {"layoutDirection", sheet.layoutDirection},
                {"wrap", sheet.flexWrap},
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

        static void from_json(const json &j, bdn::FlexStylesheet &sheet)
        {
            ALLOW_NON_EXISTING(sheet.flexDirection = j.at("direction").get<bdn::FlexStylesheet::Direction>();)
            ALLOW_NON_EXISTING(sheet.layoutDirection = j.at("layoutDirection");)
            ALLOW_NON_EXISTING(sheet.flexWrap = j.at("wrap");)
            ALLOW_NON_EXISTING(sheet.alignSelf = j.at("alignSelf");)
            ALLOW_NON_EXISTING(sheet.alignItems = j.at("alignItems");)
            ALLOW_NON_EXISTING(sheet.alignContents = j.at("alignContents");)
            ALLOW_NON_EXISTING(sheet.justifyContent = j.at("justifyContent");)
            ALLOW_NON_EXISTING(sheet.flexBasis = j.at("flexBasis").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(sheet.flexBasisPercent = j.at("flexBasisPercent").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(sheet.flexGrow = j.at("flexGrow");)
            ALLOW_NON_EXISTING(sheet.flexShrink = j.at("flexShrink");)
            ALLOW_NON_EXISTING(sheet.aspectRatio = j.at("aspectRatio").get<std::optional<float>>();)
            ALLOW_NON_EXISTING(sheet.margin = j.at("margin");)
            ALLOW_NON_EXISTING(sheet.padding = j.at("padding");)
            ALLOW_NON_EXISTING(sheet.size = j.at("size");)
            ALLOW_NON_EXISTING(sheet.maximumSize = j.at("maximumSize");)
            ALLOW_NON_EXISTING(sheet.minimumSize = j.at("minimumSize");)
        }
    };
}

namespace bdn
{
    inline FlexStylesheet FlexJson(const nlohmann::json &json) { return (FlexStylesheet)json; }
    inline FlexStylesheet FlexJsonString(const String &json) { return (FlexStylesheet)nlohmann::json::parse(json); }
}

#endif
