#include <bdn/ui/View.h>
#include <bdn/ui/yoga/FlexStylesheet.h>
#include <bdn/ui/yoga/Layout.h>

#include <yoga/YGNode.h>

namespace bdn::ui::yoga
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

    void Layout::registerView(View *view)
    {
        _views[view] = std::make_unique<ViewData>(view);
        updateStylesheet(view);
    }

    void Layout::unregisterView(View *view)
    {
        remove(view);
        auto it = _views.find(view);
        if (it != _views.end()) {
            _views.erase(it);
        }
    }

    void Layout::markDirty(View *view) { _views[view]->ygNode->markDirtyAndPropogate(); }

    void Layout::updateStylesheet(View *view) { applyStyle(view, _views[view]->ygNode); }

    void Layout::layout(View *view) { _views[view]->doLayout(); }

    FlexStylesheet fromStyleSheet(const json &jsonStyleSheet)
    {
        FlexStylesheet result;

        if (jsonStyleSheet.count("flex")) {
            result = (FlexStylesheet)jsonStyleSheet.at("flex");
        }

        return result;
    }

#define UPDATE_VALUE(FuncName, Value, ...)                                                                             \
    if (!Value) {                                                                                                      \
        FuncName(__VA_ARGS__, NAN);                                                                                    \
        FuncName##Percent(__VA_ARGS__, NAN);                                                                           \
    } else {                                                                                                           \
        if (Value->isPercent()) {                                                                                      \
            FuncName##Percent(__VA_ARGS__, Value->value);                                                              \
        } else {                                                                                                       \
            FuncName(__VA_ARGS__, Value->value);                                                                       \
        }                                                                                                              \
    }

#define UPDATE_EDGES(FuncName, Node, Value)                                                                            \
    UPDATE_VALUE(FuncName, Value.all, Node, YGEdgeAll)                                                                 \
    UPDATE_VALUE(FuncName, Value.left, Node, YGEdgeLeft)                                                               \
    UPDATE_VALUE(FuncName, Value.top, Node, YGEdgeTop)                                                                 \
    UPDATE_VALUE(FuncName, Value.right, Node, YGEdgeRight)                                                             \
    UPDATE_VALUE(FuncName, Value.bottom, Node, YGEdgeBottom)

#define UPDATE_SIZES(FuncName, Node, Value)                                                                            \
    UPDATE_VALUE(FuncName##Width, Value.width, Node)                                                                   \
    UPDATE_VALUE(FuncName##Height, Value.height, Node)

    void Layout::applyStyle(View *view, YGNodeRef ygNode)
    {
        FlexStylesheet stylesheet = fromStyleSheet(view->stylesheet.get());

        if (view->visible.get()) {
            insert(view);
        } else {
            remove(view);
        }

        YGNodeStyleSetFlexDirection(ygNode, toYGFlexDirection(stylesheet.flexDirection));
        YGNodeStyleSetDirection(ygNode, toYGDirection(stylesheet.layoutDirection));

        YGNodeStyleSetAlignContent(ygNode, toYGAlign(stylesheet.alignContents));
        YGNodeStyleSetAlignItems(ygNode, toYGAlign(stylesheet.alignItems));
        YGNodeStyleSetAlignSelf(ygNode, toYGAlign(stylesheet.alignSelf));

        YGNodeStyleSetJustifyContent(ygNode, toYGJustify(stylesheet.justifyContent));

        YGNodeStyleSetFlexWrap(ygNode, toYGWrap(stylesheet.flexWrap));

        YGNodeStyleSetFlexGrow(ygNode, stylesheet.flexGrow);
        YGNodeStyleSetFlexShrink(ygNode, stylesheet.flexShrink);

        UPDATE_EDGES(YGNodeStyleSetPadding, ygNode, stylesheet.padding)
        UPDATE_EDGES(YGNodeStyleSetMargin, ygNode, stylesheet.margin)

        UPDATE_SIZES(YGNodeStyleSet, ygNode, stylesheet.size)
        UPDATE_SIZES(YGNodeStyleSetMin, ygNode, stylesheet.minimumSize)
        UPDATE_SIZES(YGNodeStyleSetMax, ygNode, stylesheet.maximumSize)

        YGNodeStyleSetAspectRatio(ygNode, stylesheet.aspectRatio ? *stylesheet.aspectRatio : NAN);

        if (!stylesheet.flexBasis) {
            YGNodeStyleSetFlexBasisAuto(ygNode);
        } else {
            if (stylesheet.flexBasis->isPercent()) {
                YGNodeStyleSetFlexBasisPercent(ygNode, stylesheet.flexBasis->value);
            } else {
                YGNodeStyleSetFlexBasis(ygNode, stylesheet.flexBasis->value);
            }
        }
    }

    void Layout::insert(View *view)
    {
        if (std::shared_ptr<View> parent = view->getParentView()) {
            auto &viewData = _views[view];

            if (viewData->isIn)
                return;

            auto it = _views.find(parent.get());
            if (it != _views.end()) {
                viewData->isIn = true;

                auto childList = parent->childViews();

                YGNodeRemoveAllChildren(it->second->ygNode);
                YGNodeSetMeasureFunc(it->second->ygNode, nullptr);

                for (auto &child : parent->childViews()) {
                    if (child->visible.get()) {
                        auto itChild = _views.find(child.get());
                        if (itChild != _views.end()) {
                            YGNodeInsertChild(it->second->ygNode, itChild->second->ygNode,
                                              YGNodeGetChildCount(it->second->ygNode));
                        }
                    }
                }
            }
        }
    }

    void Layout::remove(View *view)
    {
        auto it = _views.find(view);
        if (it != _views.end()) {
            if (it->second->isIn) {
                if (auto owner = YGNodeGetOwner(it->second->ygNode)) {

                    auto parentData = std::find_if(_views.begin(), _views.end(), [&owner](auto &viewData) {
                        return viewData.second->ygNode == owner;
                    });

                    it->second->isIn = false;
                    YGNodeRemoveChild(owner, it->second->ygNode);
                    if (parentData != _views.end()) {
                        parentData->second->childrenChanged();
                    }
                }
            }
        }
    }
}
