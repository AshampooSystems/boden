#include <bdn/FixedView.h>
#include <bdn/Window.h>
#include <bdn/yogalayout/ViewData.h>
#include <yoga/YGNode.h>

namespace bdn
{
    namespace yogalayout
    {

        ViewData::ViewData(View *v) : view(v), isRootNode(false)
        {
            ygNode = YGNodeNew();
            YGNodeSetContext(ygNode, this);

            childrenChanged();

            if (!view->getParentView() || (dynamic_cast<FixedView *>(v) != nullptr)) {
                isRootNode = true;
                YGNodeSetDirtiedFunc(ygNode, &ViewData::onDirtied);

                if (auto window = dynamic_cast<Window *>(v)) {
                    geometry.bind(window->contentGeometry);
                } else {
                    geometry.bind(view->geometry);
                }

                geometry.onChange() += [=](auto va) { ygNode->markDirtyAndPropogate(); };
            }
        }

        ViewData::~ViewData() { YGNodeFree(ygNode); }

        void ViewData::doLayout()
        {
            if (isRootNode) {
                YGNodeCalculateLayout(ygNode, geometry->width, geometry->height, YGDirectionLTR);
                yogaVisit(ygNode, &applyLayout);

                ygNode->setDirty(false);
            }
        }

        void ViewData::onDirtied(YGNodeRef node)
        {
            auto *viewData = static_cast<ViewData *>(YGNodeGetContext(node));
            viewData->view->scheduleLayout();
        }

        static float YGSanitizeMeasurement(float constrainedSize, float measuredSize, YGMeasureMode measureMode)
        {
            float result;
            if (measureMode == YGMeasureModeExactly) {
                result = constrainedSize;
            } else if (measureMode == YGMeasureModeAtMost) {
                result = std::min(constrainedSize, measuredSize);
            } else {
                result = measuredSize;
            }

            return result;
        }

        YGSize ViewData::measureFunc(YGNodeRef node, float width, YGMeasureMode widthMode, float height,
                                     YGMeasureMode heightMode)
        {
            auto viewData = static_cast<ViewData *>(YGNodeGetContext(node));

            Size constraintSize = Size(widthMode == YGMeasureModeUndefined ? Size::componentNone() : width,
                                       heightMode == YGMeasureModeUndefined ? Size::componentNone() : height);

            Size s = viewData->view->sizeForSpace(constraintSize);

            return (YGSize){
                .width = YGSanitizeMeasurement(constraintSize.width, s.width, widthMode),
                .height = YGSanitizeMeasurement(constraintSize.height, s.height, heightMode),
            };
        }

        void ViewData::applyLayout(YGNodeRef node, Point offset)
        {
            if (auto ctxt = YGNodeGetContext(node)) {
                auto viewData = static_cast<ViewData *>(ctxt);

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
                    auto viewData = static_cast<ViewData *>(ctxt);

                    if (dynamic_cast<FixedView *>(viewData->view) != nullptr) {
                        continue;
                    }

                    function(child, initialOffset);
                    yogaVisit(child, function);
                }
            }
        }

        void ViewData::childrenChanged(bool adding)
        {
            if (YGNodeGetChildCount(ygNode) > 0 || adding) {
                YGNodeSetMeasureFunc(ygNode, nullptr);
            } else {
                YGNodeSetMeasureFunc(ygNode, &measureFunc);
            }
        }
    }
}
