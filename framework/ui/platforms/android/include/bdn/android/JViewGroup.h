#pragma once

#include <bdn/android/JView.h>
#include <bdn/android/JViewGroup__JLayoutParams.h>
#include <bdn/android/JViewGroup__JMarginLayoutParams.h>

namespace bdn
{
    namespace android
    {
        /** Wrapper for Java android.view.ViewGroup objects.*/
        constexpr const char kViewGroupClassName[] = "android/view/ViewGroup";

        template <const char *javaClassName = kViewGroupClassName, class... ConstructorArguments>
        class JBaseViewGroup : public JBaseView<javaClassName, ConstructorArguments...>
        {
          public:
            using JBaseView<javaClassName, ConstructorArguments...>::JBaseView;

            Method<void(JView)> addView{this, "addView"};
            Method<void(JView)> removeView{this, "removeView"};
            Method<void()> removeAllViews{this, "removeAllViews"};

            Method<int()> getChildCount{this, "getChildCount"};
            Method<JView(int)> getChildAt{this, "getChildAt"};

          public:
            using JLayoutParams = JViewGroup__JLayoutParams;
            using JMarginLayoutParams = JViewGroup__JMarginLayoutParams;
        };

        using JViewGroup = JBaseViewGroup<>;
    }
}
