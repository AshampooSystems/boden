#pragma once

#include <bdn/android/JNativeListAdapter.h>
#include <bdn/android/JViewGroup.h>

namespace bdn::android
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

        java::Method<void(JListAdapter)> setAdapter{this, "setAdapter"};
        java::Method<void(OnItemClickListener)> setOnItemClickListener{this, "setOnItemClickListener"};
    };
}
