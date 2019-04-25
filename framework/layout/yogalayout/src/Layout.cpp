#include <bdn/View.h>
#include <bdn/yogalayout/FlexStylesheet.h>
#include <bdn/yogalayout/Layout.h>

#include <yoga/YGNode.h>

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

            YGNodeStyleSetPadding(ygNode, YGEdgeAll, stylesheet.padding.all ? *stylesheet.padding.all : NAN);
            YGNodeStyleSetPadding(ygNode, YGEdgeLeft, stylesheet.padding.left ? *stylesheet.padding.left : NAN);
            YGNodeStyleSetPadding(ygNode, YGEdgeTop, stylesheet.padding.top ? *stylesheet.padding.top : NAN);
            YGNodeStyleSetPadding(ygNode, YGEdgeRight, stylesheet.padding.right ? *stylesheet.padding.right : NAN);
            YGNodeStyleSetPadding(ygNode, YGEdgeBottom, stylesheet.padding.bottom ? *stylesheet.padding.bottom : NAN);

            YGNodeStyleSetMargin(ygNode, YGEdgeAll, stylesheet.margin.all ? *stylesheet.margin.all : NAN);
            YGNodeStyleSetMargin(ygNode, YGEdgeLeft, stylesheet.margin.left ? *stylesheet.margin.left : NAN);
            YGNodeStyleSetMargin(ygNode, YGEdgeTop, stylesheet.margin.top ? *stylesheet.margin.top : NAN);
            YGNodeStyleSetMargin(ygNode, YGEdgeRight, stylesheet.margin.right ? *stylesheet.margin.right : NAN);
            YGNodeStyleSetMargin(ygNode, YGEdgeBottom, stylesheet.margin.bottom ? *stylesheet.margin.bottom : NAN);

            YGNodeStyleSetWidth(ygNode, stylesheet.size.width ? *stylesheet.size.width : NAN);
            YGNodeStyleSetHeight(ygNode, stylesheet.size.height ? *stylesheet.size.height : NAN);

            YGNodeStyleSetMinWidth(ygNode, stylesheet.minimumSize.width ? *stylesheet.minimumSize.width : NAN);
            YGNodeStyleSetMinHeight(ygNode, stylesheet.minimumSize.height ? *stylesheet.minimumSize.height : NAN);

            YGNodeStyleSetMaxWidth(ygNode, stylesheet.maximumSize.width ? *stylesheet.maximumSize.width : NAN);
            YGNodeStyleSetMaxHeight(ygNode, stylesheet.maximumSize.height ? *stylesheet.maximumSize.height : NAN);

            YGNodeStyleSetAspectRatio(ygNode, stylesheet.aspectRatio ? *stylesheet.aspectRatio : NAN);

            if (!stylesheet.flexBasis && !stylesheet.flexBasisPercent) {
                YGNodeStyleSetFlexBasisAuto(ygNode);
            } else if (stylesheet.flexBasis && stylesheet.flexBasisPercent) {
                throw std::runtime_error("Both flexBasis and flexBasis percent were specified!");
            } else if (stylesheet.flexBasis) {
                YGNodeStyleSetFlexBasis(ygNode, *stylesheet.flexBasis);
            } else if (stylesheet.flexBasisPercent) {
                YGNodeStyleSetFlexBasisPercent(ygNode, *stylesheet.flexBasisPercent);
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
}
