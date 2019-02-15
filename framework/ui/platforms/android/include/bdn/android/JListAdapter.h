#pragma once

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kListAdapterClassName[] = "android/widget/ListAdapter";

        template <const char *javaClassName = kListAdapterClassName, class... ConstructorArguments>
        class JListAdapterBase : public java::JTObject<javaClassName, ConstructorArguments...>
        {
          public:
            using java::JTObject<javaClassName, ConstructorArguments...>::JTObject;

          public:
            java::Method<bool()> areAllItemsEnabled{this, "areAllItemsEnabled"};
            java::Method<bool(int)> isEnabled{this, "isEnabled"};
        };

        using JListAdapter = JListAdapterBase<>;
    }
}
