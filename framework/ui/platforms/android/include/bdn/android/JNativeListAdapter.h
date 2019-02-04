#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JView.h>
#include <bdn/android/JBaseAdapter.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kNativeListAdapterClassName[] = "io/boden/android/NativeListAdapter";

        class JNativeListAdapter : public JBaseAdapter<kNativeListAdapterClassName, JView>
        {
          public:
            using JBaseAdapter<kNativeListAdapterClassName, JView>::JBaseAdapter;
        };
    }
}
