#include <bdn/yogalayout/Layout.h>

#include <bdn/View.h>

#include <yoga/YGNode.h>

namespace bdn
{
    namespace yogalayout
    {

        void Layout::registerView(View *view)
        {
            _views[view] = std::make_unique<ViewData>(view);
            updateStylesheet(view);

            if (std::shared_ptr<View> parent = view->getParentView()) {
                auto it = _views.find(parent.get());
                if (it != _views.end()) {
                    it->second->childrenChanged(true);
                    YGNodeInsertChild(it->second->ygNode, _views[view]->ygNode,
                                      YGNodeGetChildCount(it->second->ygNode));
                }
            }
        }

        void Layout::unregisterView(View *view)
        {
            auto it = _views.find(view);
            if (it != _views.end()) {

                if (auto owner = YGNodeGetOwner(it->second->ygNode)) {

                    auto parentData = std::find_if(_views.begin(), _views.end(), [&owner](auto &viewData) {
                        if (viewData.second->ygNode == owner) {
                            return true;
                        }
                        return false;
                    });
                    YGNodeRemoveChild(owner, it->second->ygNode);
                    if (parentData != _views.end()) {
                        parentData->second->childrenChanged();
                    }
                }

                _views.erase(it);
            }
        }

        void Layout::markDirty(View *view) { _views[view]->ygNode->markDirtyAndPropogate(); }

        void Layout::updateStylesheet(View *view) { applyStyle(view, _views[view]->ygNode); }

        void Layout::layout(View *view) { _views[view]->doLayout(); }

        void Layout::applyStyle(View *view, YGNodeRef ygNode)
        {
            if (std::shared_ptr<FlexStylesheet> stylesheet =
                    std::static_pointer_cast<FlexStylesheet>(view->layoutStylesheet.get())) {

                if (!stylesheet->isType(String("flex-layout"))) {
                    throw std::runtime_error("View does not have correct Style sheet type!");
                }

                YGNodeStyleSetFlexDirection(ygNode, toYGFlexDirection(stylesheet->flexDirection));
                YGNodeStyleSetDirection(ygNode, toYGDirection(stylesheet->layoutDirection));

                YGNodeStyleSetAlignContent(ygNode, toYGAlign(stylesheet->alignContents));
                YGNodeStyleSetAlignItems(ygNode, toYGAlign(stylesheet->alignItems));
                YGNodeStyleSetAlignSelf(ygNode, toYGAlign(stylesheet->alignSelf));

                YGNodeStyleSetJustifyContent(ygNode, toYGJustify(stylesheet->justifyContent));

                YGNodeStyleSetFlexWrap(ygNode, toYGWrap(stylesheet->flexWrap));

                YGNodeStyleSetFlexGrow(ygNode, stylesheet->flexGrow);
                YGNodeStyleSetFlexShrink(ygNode, stylesheet->flexShrink);

                YGNodeStyleSetPadding(ygNode, YGEdgeAll, stylesheet->padding.all ? *stylesheet->padding.all : NAN);
                YGNodeStyleSetPadding(ygNode, YGEdgeLeft, stylesheet->padding.left ? *stylesheet->padding.left : NAN);
                YGNodeStyleSetPadding(ygNode, YGEdgeTop, stylesheet->padding.top ? *stylesheet->padding.top : NAN);
                YGNodeStyleSetPadding(ygNode, YGEdgeRight,
                                      stylesheet->padding.right ? *stylesheet->padding.right : NAN);
                YGNodeStyleSetPadding(ygNode, YGEdgeBottom,
                                      stylesheet->padding.bottom ? *stylesheet->padding.bottom : NAN);

                YGNodeStyleSetMargin(ygNode, YGEdgeAll, stylesheet->margin.all ? *stylesheet->margin.all : NAN);
                YGNodeStyleSetMargin(ygNode, YGEdgeLeft, stylesheet->margin.left ? *stylesheet->margin.left : NAN);
                YGNodeStyleSetMargin(ygNode, YGEdgeTop, stylesheet->margin.top ? *stylesheet->margin.top : NAN);
                YGNodeStyleSetMargin(ygNode, YGEdgeRight, stylesheet->margin.right ? *stylesheet->margin.right : NAN);
                YGNodeStyleSetMargin(ygNode, YGEdgeBottom,
                                     stylesheet->margin.bottom ? *stylesheet->margin.bottom : NAN);

                YGNodeStyleSetWidth(ygNode, stylesheet->size.width ? *stylesheet->size.width : NAN);
                YGNodeStyleSetHeight(ygNode, stylesheet->size.height ? *stylesheet->size.height : NAN);

                YGNodeStyleSetMinWidth(ygNode, stylesheet->minimumSize.width ? *stylesheet->minimumSize.width : NAN);
                YGNodeStyleSetMinHeight(ygNode, stylesheet->minimumSize.height ? *stylesheet->minimumSize.height : NAN);

                YGNodeStyleSetMaxWidth(ygNode, stylesheet->maximumSize.width ? *stylesheet->maximumSize.width : NAN);
                YGNodeStyleSetMaxHeight(ygNode, stylesheet->maximumSize.height ? *stylesheet->maximumSize.height : NAN);

                YGNodeStyleSetAspectRatio(ygNode, stylesheet->aspectRatio ? *stylesheet->aspectRatio : NAN);

                if (!stylesheet->flexBasis && !stylesheet->flexBasisPercent) {
                    YGNodeStyleSetFlexBasisAuto(ygNode);
                } else if (stylesheet->flexBasis && stylesheet->flexBasisPercent) {
                    throw std::runtime_error("Both flexBasis and flexBasis percent were specified!");
                } else if (stylesheet->flexBasis) {
                    YGNodeStyleSetFlexBasis(ygNode, *stylesheet->flexBasis);
                } else if (stylesheet->flexBasisPercent) {
                    YGNodeStyleSetFlexBasisPercent(ygNode, *stylesheet->flexBasisPercent);
                }
            }
        }
    }
}
