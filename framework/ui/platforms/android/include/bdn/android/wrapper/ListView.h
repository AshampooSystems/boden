#pragma once

#include <bdn/android/wrapper/AdapterView.h>

namespace bdn::android::wrapper
{
    constexpr const char kListViewClassName[] = "android/widget/ListView";

    /** Accessor for Java android.widget.ListView objects.*/
    class ListView : public BaseAdapterView<kListViewClassName>
    {
      public:
        using BaseAdapterView<kListViewClassName>::BaseAdapterView;

        JavaMethod<void(int)> setChoiceMode{this, "setChoiceMode"};
        JavaMethod<void(bool)> setStackFromBottom{this, "setStackFromBottom"};
    };
}
