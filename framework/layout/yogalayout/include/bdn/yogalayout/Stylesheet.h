#pragma once

#include <bdn/FlexStylesheet.h>
#include <yoga/Yoga.h>

namespace bdn
{
    namespace yogalayout
    {
        constexpr YGFlexDirection toYGFlexDirection(FlexStylesheet::Direction direction)
        {
            switch (direction) {
            case FlexStylesheet::Direction::Column:
                return YGFlexDirectionColumn;
            case FlexStylesheet::Direction::ColumnReverse:
                return YGFlexDirectionColumnReverse;
            case FlexStylesheet::Direction::Row:
                return YGFlexDirectionRow;
            case FlexStylesheet::Direction::RowReverse:
                return YGFlexDirectionRowReverse;
            }
            return YGFlexDirectionColumn;
        }

        constexpr YGDirection toYGDirection(FlexStylesheet::LayoutDirection direction)
        {
            switch (direction) {
            case FlexStylesheet::LayoutDirection::Inherit:
                return YGDirectionInherit;
            case FlexStylesheet::LayoutDirection::LTR:
                return YGDirectionLTR;
            case FlexStylesheet::LayoutDirection::RTL:
                return YGDirectionRTL;
            }
            return YGDirectionInherit;
        }

        constexpr YGAlign toYGAlign(FlexStylesheet::Align align)
        {
            switch (align) {
            case FlexStylesheet::Align::Auto:
                return YGAlignAuto;
            case FlexStylesheet::Align::FlexStart:
                return YGAlignFlexStart;
            case FlexStylesheet::Align::Center:
                return YGAlignCenter;
            case FlexStylesheet::Align::FlexEnd:
                return YGAlignFlexEnd;
            case FlexStylesheet::Align::Stretch:
                return YGAlignStretch;
            case FlexStylesheet::Align::Baseline:
                return YGAlignBaseline;
            case FlexStylesheet::Align::SpaceBetween:
                return YGAlignSpaceBetween;
            case FlexStylesheet::Align::SpaceAround:
                return YGAlignSpaceAround;
            }
            return YGAlignAuto;
        }

        constexpr YGJustify toYGJustify(FlexStylesheet::Justify justify)
        {
            switch (justify) {
            case FlexStylesheet::Justify::FlexStart:
                return YGJustifyFlexStart;
            case FlexStylesheet::Justify::Center:
                return YGJustifyCenter;
            case FlexStylesheet::Justify::FlexEnd:
                return YGJustifyFlexEnd;
            case FlexStylesheet::Justify::SpaceBetween:
                return YGJustifySpaceBetween;
            case FlexStylesheet::Justify::SpaceAround:
                return YGJustifySpaceAround;
            case FlexStylesheet::Justify::SpaceEvenly:
                return YGJustifySpaceEvenly;
            }
            return YGJustifyFlexStart;
        }

        constexpr YGWrap toYGWrap(FlexStylesheet::Wrap wrap)
        {
            switch (wrap) {
            case FlexStylesheet::Wrap::NoWrap:
                return YGWrapNoWrap;
            case FlexStylesheet::Wrap::Wrap:
                return YGWrapWrap;
            case FlexStylesheet::Wrap::WrapReverse:
                return YGWrapWrapReverse;
            }

            return YGWrapNoWrap;
        }
    }
}
