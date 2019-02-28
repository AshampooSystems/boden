#include <bdn/yogalayout/ViewData.h>

#include <bdn/ContainerView.h>
#include <bdn/FixedView.h>
#include <bdn/ListView.h>
#include <bdn/ScrollView.h>
#include <bdn/Stack.h>
#include <bdn/View.h>
#include <bdn/Window.h>

#include <yoga/YGNode.h>

namespace bdn
{
    namespace yogalayout
    {

        ViewData::ViewData(View *v) : view(v), isRootNode(false)
        {
            ygNode = YGNodeNew();
            YGNodeSetContext(ygNode, this);

            if (!dynamic_cast<ContainerView *>(v) && !dynamic_cast<Window *>(view) &&
                !dynamic_cast<ScrollView *>(view) && !dynamic_cast<Stack *>(view) && !dynamic_cast<ListView *>(view)) {
                YGNodeSetMeasureFunc(ygNode, &measureFunc);
            }

            if (!view->getParentView() || dynamic_cast<FixedView *>(v)) {
                isRootNode = true;
                YGNodeSetDirtiedFunc(ygNode, &ViewData::onDirtied);

                if (auto window = dynamic_cast<Window *>(v)) {
                    geometry.bind(window->contentGeometry, BindMode::unidirectional);
                } else {
                    geometry.bind(view->geometry, BindMode::unidirectional);
                }

                geometry.onChange() += [=](auto va) { ygNode->markDirtyAndPropogate(); };
            }
        }

        ViewData::~ViewData() { YGNodeFree(ygNode); }

        void ViewData::doLayout()
        {
            if (isRootNode) {
                YGNodeCalculateLayout(ygNode, geometry->width, geometry->height, YGDirectionLTR);
                yogaVisit(ygNode, &applyLayout, geometry->getPosition());

                ygNode->setDirty(false);
            }
        }

        void ViewData::onDirtied(YGNodeRef node)
        {
            ViewData *viewData = (ViewData *)YGNodeGetContext(node);
            viewData->view->scheduleLayout();
        }

        YGSize ViewData::measureFunc(YGNodeRef node, float width, YGMeasureMode widthMode, float height,
                                     YGMeasureMode heightMode)
        {
            auto viewData = (ViewData *)YGNodeGetContext(node);

            Size s = viewData->view->sizeForSpace({width, height});

            return YGSize{(float)s.width, (float)s.height};
        }

        void ViewData::applyLayout(YGNodeRef node, Point offset)
        {
            if (auto ctxt = YGNodeGetContext(node)) {
                auto viewData = (ViewData *)ctxt;

                Rect r{YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node), YGNodeLayoutGetWidth(node),
                       YGNodeLayoutGetHeight(node)};

                if (std::isnan(r.x)) {
                    r.x = 0;
                }
                if (std::isnan(r.y)) {
                    r.y = 0;
                }
                if (std::isnan(r.width)) {
                    r.width = 0;
                }
                if (std::isnan(r.height)) {
                    r.height = 0;
                }

                r.x += offset.x;
                r.y += offset.y;

                viewData->view->geometry = r;
            }
        }

        void ViewData::yogaVisit(YGNodeRef node, const std::function<void(YGNodeRef, Point)> &function,
                                 Point initialOffset)
        {
            for (int i = 0; i < YGNodeGetChildCount(node); i++) {
                auto child = YGNodeGetChild(node, i);

                if (auto ctxt = YGNodeGetContext(child)) {
                    auto viewData = (ViewData *)ctxt;

                    if (dynamic_cast<FixedView *>(viewData->view)) {
                        continue;
                    }

                    function(child, initialOffset);
                    yogaVisit(child, function);
                }
            }
        }
    }
}
