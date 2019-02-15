#pragma once

#include <bdn/android/JBaseAdapter.h>
#include <bdn/android/JView.h>
#include <bdn/java/JObject.h>

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
