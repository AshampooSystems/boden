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
    };
}
