#pragma once

#include <bdn/android/wrapper/BaseAdapter.h>
#include <bdn/android/wrapper/View.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeListAdapterClassName[] = "io/boden/android/NativeListAdapter";

    class NativeListAdapter : public BaseAdapter<kNativeListAdapterClassName, View>
    {
      public:
        using BaseAdapter<kNativeListAdapterClassName, View>::BaseAdapter;
    };
}
