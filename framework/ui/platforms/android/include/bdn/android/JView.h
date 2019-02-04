#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JContext.h>
#include <bdn/android/JIBinder.h>
#include <bdn/android/JViewParent.h>

#include <bdn/android/JViewGroup__JLayoutParams.h>

namespace bdn
{
    namespace android
    {
        using namespace java;

        constexpr const char kOnClickListenerClassName[] = "android/view/View$OnClickListener";
        template <const char *javaClassName = kOnClickListenerClassName, class... ConstructorArguments>
        class OnClickListenerBase : public JTObject<javaClassName, ConstructorArguments...>
        {
          public:
            using JTObject<javaClassName, ConstructorArguments...>::JTObject;

            template <class Other> OnClickListenerBase(Other &other) : OnClickListenerBase(other.getRef_()) {}
        };

        using OnClickListener = OnClickListenerBase<>;

        constexpr const char kOnScrollChangeListenerClassName[] = "android/view/View$OnScrollChangeListener";
        using OnScrollChangeListener = JTObject<kOnScrollChangeListenerClassName>;

        constexpr const char kOnLayoutChangeListenerClassName[] = "android/view/View$OnLayoutChangeListener";
        using OnLayoutChangeListener = JTObject<kOnLayoutChangeListenerClassName>;

        constexpr const char kViewClassName[] = "android/view/View";

        template <const char *javaClassName = kViewClassName, class... ConstructorArguments>
        class JBaseView : public JTObject<javaClassName, JContext, ConstructorArguments...>
        {
          public:
            using JTObject<javaClassName, JContext, ConstructorArguments...>::JTObject;

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
            Method<void(int)> setVisibility{this, "setVisibility"};
            Method<int()> getVisibility{this, "getVisibility"};

            Method<int()> getLeft{this, "getLeft"};
            Method<int()> getTop{this, "getTop"};
            Method<int()> getWidth{this, "getWidth"};
            Method<int()> getHeight{this, "getHeight"};

            Method<void(int, int, int, int)> setPadding{this, "setPadding"};

            Method<int()> getPaddingLeft{this, "getPaddingLeft"};
            Method<int()> getPaddingTop{this, "getPaddingTop"};
            Method<int()> getPaddingRight{this, "getPaddingRight"};
            Method<int()> getPaddingBottom{this, "getPaddingBottom"};

            Method<JViewParent()> getParent{this, "getParent"};
            Method<JContext()> getContext{this, "getContext"};

            Method<void(JObject)> setTag{this, "setTag"};
            Method<JObject()> getTag{this, "getTag"};

            Method<void(int, int)> measure{this, "measure"};

            Method<int()> getMeasuredWidth{this, "getMeasuredWidth"};
            Method<int()> getMeasuredHeight{this, "getMeasuredHeight"};

            Method<void()> requestLayout{this, "requestLayout"};
            Method<void()> invalidate{this, "invalidate"};

            Method<int()> getSuggestedMinimumWidth{this, "getSuggestedMinimumWidth"};
            Method<int()> getSuggestedMinimumHeight{this, "getSuggestedMinimumHeight"};

            Method<int()> getScrollX{this, "getScrollX"};
            Method<int()> getScrollY{this, "getScrollY"};

            Method<void(OnClickListener)> setOnClickListener{this, "setOnClickListener"};
            Method<void(OnScrollChangeListener)> setOnScrollChangeListener{this, "setOnScrollChangeListener"};
            Method<void(OnLayoutChangeListener)> addOnLayoutChangeListener{this, "addOnLayoutChangeListener"};

            Method<void(JViewGroup__JLayoutParams)> setLayoutParams{this, "setLayoutParams"};

            Method<JIBinder()> getWindowToken{this, "getWindowToken"};
        };

        using JView = JBaseView<>;
    }
}
