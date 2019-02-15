#pragma once

#include <bdn/android/JContext.h>
#include <bdn/android/JIBinder.h>
#include <bdn/android/JViewParent.h>
#include <bdn/java/JObject.h>

#include <bdn/android/JViewGroup__JLayoutParams.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kOnClickListenerClassName[] = "android/view/View$OnClickListener";
        template <const char *javaClassName = kOnClickListenerClassName, class... ConstructorArguments>
        class OnClickListenerBase : public java::JTObject<javaClassName, ConstructorArguments...>
        {
          public:
            using java::JTObject<javaClassName, ConstructorArguments...>::JTObject;

            template <class Other> OnClickListenerBase(Other &other) : OnClickListenerBase(other.getRef_()) {}
        };

        using OnClickListener = OnClickListenerBase<>;

        constexpr const char kOnScrollChangeListenerClassName[] = "android/view/View$OnScrollChangeListener";
        using OnScrollChangeListener = java::JTObject<kOnScrollChangeListenerClassName>;

        constexpr const char kOnLayoutChangeListenerClassName[] = "android/view/View$OnLayoutChangeListener";
        using OnLayoutChangeListener = java::JTObject<kOnLayoutChangeListenerClassName>;

        constexpr const char kViewClassName[] = "android/view/View";

        template <const char *javaClassName = kViewClassName, class... ConstructorArguments>
        class JBaseView : public java::JTObject<javaClassName, JContext, ConstructorArguments...>
        {
          public:
            using java::JTObject<javaClassName, JContext, ConstructorArguments...>::JTObject;

          public:
            enum Visibility
            {
                visible = 0,

                /** The view is invisible, but still takes up space during
                   layout.*/
                invisible = 4,

                /** The view is invisible and does NOT take up space during
                   layout.*/
                gone = 8
            };

            enum SYSTEM_UI_FLAG
            {
                VISIBLE = 0,
                LOW_PROFILE = 0x00000001,
                HIDE_NAVIGATION = 0x00000002,
                FULLSCREEN = 0x00000004,
                LAYOUT_STABLE = 0x00000100,
                LAYOUT_HIDE_NAVIGATION = 0x00000200,
                LAYOUT_FULLSCREEN = 0x00000400,
                IMMERSIVE = 0x00000800,
                IMMERSIVE_STICKY = 0x00001000,
                LIGHT_STATUS_BAR = 0x00002000,
                LIGHT_NAVIGATION_BAR = 0x00000010
            };

            class MeasureSpec
            {
              public:
                enum Mode
                {
                    /** Indicates that the view can be as large as it wants up
                       to the specified size.*/
                    unspecified = 0,

                    /** Indicates that the view must be exactly the given size.
                     * */
                    exactly = 0x40000000,

                    /** Indicates that there is no size constraint on the
                       view.*/
                    atMost = 0x80000000
                };

                static int makeMeasureSpec(int size, Mode mode) { return size | mode; }
            };

          public:
            java::Method<void(int)> setSystemUiVisibility{this, "setSystemUiVisibility"};
            java::Method<int()> getSystemUiVisibility{this, "getSystemUiVisibility"};

            java::Method<void(int)> setVisibility{this, "setVisibility"};
            java::Method<int()> getVisibility{this, "getVisibility"};

            java::Method<int()> getLeft{this, "getLeft"};
            java::Method<int()> getTop{this, "getTop"};
            java::Method<int()> getWidth{this, "getWidth"};
            java::Method<int()> getHeight{this, "getHeight"};

            java::Method<void(int, int, int, int)> setPadding{this, "setPadding"};

            java::Method<int()> getPaddingLeft{this, "getPaddingLeft"};
            java::Method<int()> getPaddingTop{this, "getPaddingTop"};
            java::Method<int()> getPaddingRight{this, "getPaddingRight"};
            java::Method<int()> getPaddingBottom{this, "getPaddingBottom"};

            java::Method<JViewParent()> getParent{this, "getParent"};
            java::Method<JContext()> getContext{this, "getContext"};

            java::Method<void(java::JObject)> setTag{this, "setTag"};
            java::Method<java::JObject()> getTag{this, "getTag"};

            java::Method<void(int, int)> measure{this, "measure"};

            java::Method<int()> getMeasuredWidth{this, "getMeasuredWidth"};
            java::Method<int()> getMeasuredHeight{this, "getMeasuredHeight"};

            java::Method<void()> requestLayout{this, "requestLayout"};
            java::Method<void()> invalidate{this, "invalidate"};

            java::Method<int()> getSuggestedMinimumWidth{this, "getSuggestedMinimumWidth"};
            java::Method<int()> getSuggestedMinimumHeight{this, "getSuggestedMinimumHeight"};

            java::Method<int()> getScrollX{this, "getScrollX"};
            java::Method<int()> getScrollY{this, "getScrollY"};

            java::Method<void(OnClickListener)> setOnClickListener{this, "setOnClickListener"};
            java::Method<void(OnScrollChangeListener)> setOnScrollChangeListener{this, "setOnScrollChangeListener"};
            java::Method<void(OnLayoutChangeListener)> addOnLayoutChangeListener{this, "addOnLayoutChangeListener"};

            java::Method<void(JViewGroup__JLayoutParams)> setLayoutParams{this, "setLayoutParams"};

            java::Method<JIBinder()> getWindowToken{this, "getWindowToken"};
        };

        using JView = JBaseView<>;
    }
}
