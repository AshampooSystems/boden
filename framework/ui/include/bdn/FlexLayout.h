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

        struct
        {
            std::optional<float> all;
            std::optional<float> left;
            std::optional<float> top;
            std::optional<float> right;
            std::optional<float> bottom;
        } padding;

        struct
        {
            std::optional<float> all;
            std::optional<float> left;
            std::optional<float> top;
            std::optional<float> right;
            std::optional<float> bottom;
        } margin;

        struct
        {
            std::optional<float> width;
            std::optional<float> height;
        } size;

        struct
        {
            std::optional<float> width;
            std::optional<float> height;
        } minimumSize;

        struct
        {
            std::optional<float> width;
            std::optional<float> height;
        } maximumSize;

        std::optional<float> aspectRatio;

        template <class Manip> FlexStylesheet &operator<<(const Manip &manipulator)
        {
            manipulator.apply(this);
            return *this;
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
