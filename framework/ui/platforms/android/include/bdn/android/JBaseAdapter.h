#pragma once

#include <bdn/android/JListAdapter.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kBaseAdapterClassName[] = "android/widget/BaseAdapter";

        template <const char *javaClassName = kBaseAdapterClassName, class... ConstructorArguments>
        class JBaseAdapter : public JListAdapterBase<javaClassName, ConstructorArguments...>
        {
          public:
            using JListAdapterBase<javaClassName, ConstructorArguments...>::JListAdapterBase;

          public:
            java::Method<void()> notifyDataSetChanged{this, "notifyDataSetChanged"};
        };
    }
}
