#pragma once

#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/IBinder.h>
#include <bdn/android/wrapper/ViewParent.h>
#include <bdn/java/wrapper/Object.h>

#include <bdn/android/wrapper/ViewGroup__LayoutParams.h>

namespace bdn::android::wrapper
{
    constexpr const char kOnClickListenerClassName[] = "android/view/View$OnClickListener";
    template <const char *javaClassName = kOnClickListenerClassName, class... ConstructorArguments>
    class OnClickListenerBase : public java::wrapper::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, ConstructorArguments...>::JTObject;

        template <class Other> OnClickListenerBase(Other &other) : OnClickListenerBase(other.getRef_()) {}
    };

    using OnClickListener = OnClickListenerBase<>;

    constexpr const char kOnLayoutChangeListenerClassName[] = "android/view/View$OnLayoutChangeListener";
    template <const char *javaClassName = kOnLayoutChangeListenerClassName, class... ConstructorArguments>
    class OnLayoutChangeListenerBase : public java::wrapper::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, ConstructorArguments...>::JTObject;

        template <class Other> OnLayoutChangeListenerBase(Other &other) : OnLayoutChangeListenerBase(other.getRef_()) {}
    };
    using OnLayoutChangeListener = OnLayoutChangeListenerBase<>;

    constexpr const char kOnScrollChangeListenerClassName[] = "android/view/View$OnScrollChangeListener";
    using OnScrollChangeListener = java::wrapper::JTObject<kOnScrollChangeListenerClassName>;

    constexpr const char kViewClassName[] = "android/view/View";

    template <const char *javaClassName = kViewClassName, class... ConstructorArguments>
    class BaseView : public java::wrapper::JTObject<javaClassName, Context, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, Context, ConstructorArguments...>::JTObject;

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
        JavaMethod<void(int)> setSystemUiVisibility{this, "setSystemUiVisibility"};
        JavaMethod<int()> getSystemUiVisibility{this, "getSystemUiVisibility"};

        JavaMethod<void(int)> setBackgroundColor{this, "setBackgroundColor"};

        JavaMethod<void(int)> setVisibility{this, "setVisibility"};
        JavaMethod<int()> getVisibility{this, "getVisibility"};

        JavaMethod<int()> getLeft{this, "getLeft"};
        JavaMethod<int()> getTop{this, "getTop"};
        JavaMethod<int()> getWidth{this, "getWidth"};
        JavaMethod<int()> getHeight{this, "getHeight"};

        JavaMethod<void(int, int, int, int)> setPadding{this, "setPadding"};

        JavaMethod<int()> getPaddingLeft{this, "getPaddingLeft"};
        JavaMethod<int()> getPaddingTop{this, "getPaddingTop"};
        JavaMethod<int()> getPaddingRight{this, "getPaddingRight"};
        JavaMethod<int()> getPaddingBottom{this, "getPaddingBottom"};

        JavaMethod<ViewParent()> getParent{this, "getParent"};
        JavaMethod<Context()> getContext{this, "getContext"};

        JavaMethod<void(const JavaObject &)> setTag{this, "setTag"};
        JavaMethod<JavaObject()> getTag{this, "getTag"};

        JavaMethod<void(int, int)> measure{this, "measure"};

        JavaMethod<int()> getMeasuredWidth{this, "getMeasuredWidth"};
        JavaMethod<int()> getMeasuredHeight{this, "getMeasuredHeight"};

        JavaMethod<void(int, int, int, int)> layout{this, "layout"};

        JavaMethod<void()> requestLayout{this, "requestLayout"};
        JavaMethod<void()> invalidate{this, "invalidate"};

        JavaMethod<int()> getSuggestedMinimumWidth{this, "getSuggestedMinimumWidth"};
        JavaMethod<int()> getSuggestedMinimumHeight{this, "getSuggestedMinimumHeight"};

        JavaMethod<int()> getScrollX{this, "getScrollX"};
        JavaMethod<int()> getScrollY{this, "getScrollY"};

        JavaMethod<void(OnClickListener)> setOnClickListener{this, "setOnClickListener"};
        JavaMethod<void(OnScrollChangeListener)> setOnScrollChangeListener{this, "setOnScrollChangeListener"};
        JavaMethod<void(OnLayoutChangeListener)> addOnLayoutChangeListener{this, "addOnLayoutChangeListener"};

        JavaMethod<void(ViewGroup__LayoutParams)> setLayoutParams{this, "setLayoutParams"};

        JavaMethod<IBinder()> getWindowToken{this, "getWindowToken"};
    };

    using View = BaseView<>;
}
