#pragma once

#include <bdn/android/wrapper/BaseAdapter.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>
#include <bdn/java/wrapper/NativeStrongPointer.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeListAdapterClassName[] = "io/boden/android/NativeListAdapter";

    class NativeListAdapter : public BaseAdapter<kNativeListAdapterClassName, View>
    {
      public:
        using BaseAdapter<kNativeListAdapterClassName, View>::BaseAdapter;
    };

    constexpr const char kNativeListAdapterRowContainer[] = "io/boden/android/RowContainer";
    class NativeListAdapterRowContainer : public BaseNativeViewGroup<kNativeListAdapterRowContainer>
    {
      public:
        using BaseNativeViewGroup<kNativeListAdapterRowContainer>::BaseNativeViewGroup;

      public:
        JavaMethod<void(java::wrapper::NativeStrongPointer)> setBdnView{this, "setBdnView"};
        JavaMethod<java::wrapper::NativeStrongPointer()> getBdnView{this, "getBdnView"};
    };
}
