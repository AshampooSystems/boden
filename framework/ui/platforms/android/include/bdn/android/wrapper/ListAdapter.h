#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kListAdapterClassName[] = "android/widget/ListAdapter";

    template <const char *javaClassName = kListAdapterClassName, class... ConstructorArguments>
    class ListAdapterBase : public java::wrapper::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, ConstructorArguments...>::JTObject;

      public:
        JavaMethod<bool()> areAllItemsEnabled{this, "areAllItemsEnabled"};
        JavaMethod<bool(int)> isEnabled{this, "isEnabled"};
    };

    using ListAdapter = ListAdapterBase<>;
}
