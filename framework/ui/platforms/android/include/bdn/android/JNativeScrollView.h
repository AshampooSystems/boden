#pragma once

#include <bdn/android/JNativeViewGroup.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kNativeScrollViewClassName[] = "io/boden/android/NativeScrollView";

        class JNativeScrollView : public JTObject<kNativeScrollViewClassName, JContext>
        {
          public:
            using JTObject<kNativeScrollViewClassName, JContext>::JTObject;

            /** Returns the outermost view object that wraps the whole scroll
             * view.
             *
             *  This view can contain several nested sub views. Use
             * getContentParent() to get the view that acts as the parent for
             * the content view.
             * */
            Method<JView()> getWrapperView{this, "getWrapperView"};

            /** Returns the view group that is the parent of the scroll view's
             * content view. This NativeViewGroup acts as the glue between our
             * layout system and the scroll view's internal workings from
             * android.*/
            Method<JNativeViewGroup()> getContentParent{this, "getContentParent"};

            /** Scrolls to the specified scroll position (without animation)*/
            Method<void(int, int)> scrollTo{this, "scrollTo"};

            /** Scrolls to the specified scroll position with a smooth
             * animation*/
            Method<void(int, int)> smoothScrollTo{this, "smoothScrollTo"};

            /** Returns the X coordinate of the scroll position.*/
            Method<int()> getScrollX{this, "getScrollX"};
            /** Returns the Y coordinate of the scroll position.*/
            Method<int()> getScrollY{this, "getScrollY"};

            /** Returns the width of the scroll view.*/
            Method<int()> getWidth{this, "getWidth"};

            /** Returns the height of the scroll view.*/
            Method<int()> getHeight{this, "getHeight"};
        };
    }
}
