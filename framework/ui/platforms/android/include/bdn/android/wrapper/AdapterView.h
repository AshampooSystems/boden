#pragma once

#include <bdn/android/wrapper/ListAdapter.h>
#include <bdn/android/wrapper/ViewGroup.h>

namespace bdn::android::wrapper
{
    constexpr const char kAdapterViewClassName[] = "android/widget/AdapterView";

    static constexpr char kOnItemClickListener[] = "android/widget/AdapterView$OnItemClickListener";
    using OnItemClickListener = bdn::java::wrapper::JTObject<kOnItemClickListener>;

    /** Accessor for Java android.widget.AdapterView objects.*/
    template <const char *javaClassName = kAdapterViewClassName>
    class BaseAdapterView : public BaseViewGroup<javaClassName>
    {
      public:
        using BaseViewGroup<javaClassName>::BaseViewGroup;
        using AdapterView = BaseViewGroup<>;

        JavaMethod<void(ListAdapter)> setAdapter{this, "setAdapter"};
        JavaMethod<void(OnItemClickListener)> setOnItemClickListener{this, "setOnItemClickListener"};
    };
}
