#pragma once

#include <bdn/android/wrapper/ListAdapter.h>

namespace bdn::android::wrapper
{
    constexpr const char kBaseAdapterClassName[] = "android/widget/BaseAdapter";

    template <const char *javaClassName = kBaseAdapterClassName, class... ConstructorArguments>
    class BaseAdapter : public ListAdapterBase<javaClassName, ConstructorArguments...>
    {
      public:
        using ListAdapterBase<javaClassName, ConstructorArguments...>::ListAdapterBase;

      public:
        JavaMethod<void()> notifyDataSetChanged{this, "notifyDataSetChanged"};
    };
}
