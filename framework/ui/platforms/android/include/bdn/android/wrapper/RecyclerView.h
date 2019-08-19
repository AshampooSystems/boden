#pragma once

#include <bdn/android/wrapper/ViewGroup.h>

namespace bdn::android::wrapper
{
    constexpr const char kRecyclerViewClassName[] = "androidx/recyclerview/widget/RecyclerView";
    constexpr const char kRecyclerViewAdapterClassName[] = "androidx/recyclerview/widget/RecyclerView$Adapter";

    class RecyclerView : public BaseViewGroup<kRecyclerViewClassName>
    {
      public:
        using BaseViewGroup<kRecyclerViewClassName>::BaseViewGroup;

      public:
        template <const char *javaClassName = kRecyclerViewAdapterClassName, class... ConstructorArguments>
        class BaseAdapter : public JTObject<javaClassName, ConstructorArguments...>
        {
          public:
            using JTObject<javaClassName, ConstructorArguments...>::JTObject;

          public:
            JavaMethod<void()> notifyDataSetChanged{this, "notifyDataSetChanged"};
        };

        using Adapter = BaseAdapter<>;

      public:
        JavaMethod<void(Adapter)> setAdapter{this, "setAdapter"};
    };
}
