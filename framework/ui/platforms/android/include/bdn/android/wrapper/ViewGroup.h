#pragma once

#include <bdn/android/wrapper/View.h>
#include <bdn/java/StaticField.h>

namespace bdn::android::wrapper
{
    /** Wrapper for Java android.view.ViewGroup objects.*/
    constexpr const char kViewGroupClassName[] = "android/view/ViewGroup";

    template <const char *javaClassName = kViewGroupClassName, class... ConstructorArguments>
    class BaseViewGroup : public BaseView<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseView<javaClassName, ConstructorArguments...>::BaseView;

        JavaMethod<void(View)> addView{this, "addView"};
        JavaMethod<void(View)> removeView{this, "removeView"};
        JavaMethod<void()> removeAllViews{this, "removeAllViews"};

        JavaMethod<int()> getChildCount{this, "getChildCount"};
        JavaMethod<View(int)> getChildAt{this, "getChildAt"};
        JavaMethod<void(int)> setDescendantFocusability{this, "setDescendantFocusability"};
    };

    using ViewGroup = BaseViewGroup<>;
}
