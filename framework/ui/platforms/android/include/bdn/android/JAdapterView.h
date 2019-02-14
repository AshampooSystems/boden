#pragma once

#include <bdn/android/JViewGroup.h>
#include <bdn/android/JNativeListAdapter.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kAdapterViewClassName[] = "android/widget/AdapterView";

        static constexpr char kOnItemClickListener[] = "android/widget/AdapterView$OnItemClickListener";
        using OnItemClickListener = bdn::java::JTObject<kOnItemClickListener>;

        /** Accessor for Java android.widget.AdapterView objects.*/
        template <const char *javaClassName = kAdapterViewClassName>
        class JBaseAdapterView : public JBaseViewGroup<javaClassName>
        {
          public:
            using JBaseViewGroup<javaClassName>::JBaseViewGroup;
            using JAdapterView = JBaseViewGroup<>;

            Method<void(JListAdapter)> setAdapter{this, "setAdapter"};
            Method<void(OnItemClickListener)> setOnItemClickListener{this, "setOnItemClickListener"};
        };
    }
}
