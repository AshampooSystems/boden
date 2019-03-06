#pragma once

#include <bdn/android/wrapper/NativeViewGroup.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeScrollViewClassName[] = "io/boden/android/NativeScrollView";

    class NativeScrollView : public java::wrapper::JTObject<kNativeScrollViewClassName, Context>
    {
      public:
        using java::wrapper::JTObject<kNativeScrollViewClassName, Context>::JTObject;

        /** Returns the outermost view object that wraps the whole scroll
         * view.
         *
         *  This view can contain several nested sub views. Use
         * getContentParent() to get the view that acts as the parent for
         * the content view.
         * */
        JavaMethod<View()> getWrapperView{this, "getWrapperView"};

        /** Returns the view group that is the parent of the scroll view's
         * content view. This NativeViewGroup acts as the glue between our
         * layout system and the scroll view's internal workings from
         * android.*/
        JavaMethod<NativeViewGroup()> getContentParent{this, "getContentParent"};

        /** Scrolls to the specified scroll position (without animation)*/
        JavaMethod<void(int, int)> scrollTo{this, "scrollTo"};

        /** Scrolls to the specified scroll position with a smooth
         * animation*/
        JavaMethod<void(int, int)> smoothScrollTo{this, "smoothScrollTo"};

        /** Returns the X coordinate of the scroll position.*/
        JavaMethod<int()> getScrollX{this, "getScrollX"};
        /** Returns the Y coordinate of the scroll position.*/
        JavaMethod<int()> getScrollY{this, "getScrollY"};

        /** Returns the width of the scroll view.*/
        JavaMethod<int()> getWidth{this, "getWidth"};

        /** Returns the height of the scroll view.*/
        JavaMethod<int()> getHeight{this, "getHeight"};
    };
}
