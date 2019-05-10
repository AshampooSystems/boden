#pragma once

#include <bdn/Point.h>
#include <bdn/Rect.h>
#include <bdn/property/Property.h>
#include <bdn/ui/View.h>
#include <yoga/Yoga.h>

struct YGNode;

namespace bdn::ui::yoga
{
    class ViewData
    {
      public:
        ViewData(View *v);
        ~ViewData();

        void doLayout();

        static void onDirtied(YGNodeRef node);

        static YGSize measureFunc(YGNodeRef node, float width, YGMeasureMode widthMode, float height,
                                  YGMeasureMode heightMode);

        static void applyLayout(YGNodeRef node, Point offset);

        static void yogaVisit(YGNodeRef node, const std::function<void(YGNodeRef, Point)> &function,
                              Point initialOffset = {0, 0});

        void childrenChanged(bool adding = false);

      public:
        Property<Rect> geometry;

        YGNodeRef ygNode;
        View *view;
        std::function<void()> layoutFunction;
        bool isRootNode;
        bool isIn;
    };
}
