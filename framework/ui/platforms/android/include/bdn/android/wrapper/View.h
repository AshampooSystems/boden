#pragma once

#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/IBinder.h>
#include <bdn/android/wrapper/ViewParent.h>
#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

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

    constexpr const char kViewGroupLayoutParamsClassName[] = "android/view/ViewGroup$LayoutParams";

    class LayoutParams : public java::wrapper::JTObject<kViewGroupLayoutParamsClassName, int, int>
    {
      public:
        using java::wrapper::JTObject<kViewGroupLayoutParamsClassName, int, int>::JTObject;

      public:
        java::StaticFinalField<int, LayoutParams> MATCH_PARENT{"MATCH_PARENT"};
        java::StaticFinalField<int, LayoutParams> WRAP_CONTENT{"WRAP_CONTENT"};

      public:
        java::ObjectField<int> width{this, "width"};
        java::ObjectField<int> height{this, "height"};
    };

    constexpr const char kViewClassName[] = "android/view/View";
    constexpr const char kViewMeasureSpecClassName[] = "android/view/View$MeasureSpec";

    template <const char *javaClassName = kViewClassName, class... ConstructorArguments>
    class BaseView : public java::wrapper::JTObject<javaClassName, Context, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, Context, ConstructorArguments...>::JTObject;

      public:
        constexpr static java::StaticFinalField<int, BaseView> VISIBLE{"VISIBLE"};
        constexpr static java::StaticFinalField<int, BaseView> INVISIBLE{"INVISIBLE"};
        constexpr static java::StaticFinalField<int, BaseView> GONE{"GONE"};

        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_VISIBLE{"SYSTEM_UI_FLAG_VISIBLE"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_LOW_PROFILE{"SYSTEM_UI_FLAG_LOW_PROFILE"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_HIDE_NAVIGATION{
            "SYSTEM_UI_FLAG_HIDE_NAVIGATION"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_FULLSCREEN{"SYSTEM_UI_FLAG_FULLSCREEN"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_LAYOUT_STABLE{
            "SYSTEM_UI_FLAG_LAYOUT_STABLE"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION{
            "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN{
            "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_IMMERSIVE{"SYSTEM_UI_FLAG_IMMERSIVE"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_IMMERSIVE_STICKY{
            "SYSTEM_UI_FLAG_IMMERSIVE_STICKY"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_LIGHT_STATUS_BAR{
            "SYSTEM_UI_FLAG_LIGHT_STATUS_BAR"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_RESERVED_LEGACY1{"SYSTEM_UI_RESERVED_LEGACY1"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_RESERVED_LEGACY2{"SYSTEM_UI_RESERVED_LEGACY2"};
        constexpr static java::StaticFinalField<int, BaseView> SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR{
            "SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR"};

        class MeasureSpec : public java::wrapper::JTObject<kViewMeasureSpecClassName>
        {
          public:
            using JTObject<kViewMeasureSpecClassName>::JTObject;

          public:
            constexpr static java::StaticFinalField<int, MeasureSpec> UNSPECIFIED{"UNSPECIFIED"};
            constexpr static java::StaticFinalField<int, MeasureSpec> EXACTLY{"EXACTLY"};
            constexpr static java::StaticFinalField<int, MeasureSpec> AT_MOST{"AT_MOST"};

            static int makeMeasureSpec(int size, int mode) { return size | mode; }
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

        JavaMethod<int()> getBaseline{this, "getBaseline"};

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

        JavaMethod<void(LayoutParams)> setLayoutParams{this, "setLayoutParams"};

        JavaMethod<IBinder()> getWindowToken{this, "getWindowToken"};
    };

    using View = BaseView<>;
}
