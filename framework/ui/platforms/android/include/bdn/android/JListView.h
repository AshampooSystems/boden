#pragma once

#include <bdn/android/JAdapterView.h>

namespace bdn
{
    namespace android
    {

        constexpr const char kListViewClassName[] = "android/widget/ListView";

        /** Accessor for Java android.widget.ListView objects.*/
        class JListView : public JBaseAdapterView<kListViewClassName>
        {
          public:
            using JBaseAdapterView<kListViewClassName>::JBaseAdapterView;

            java::Method<void(int)> setDescendantFocusability{this, "setDescendantFocusability"};
            java::Method<void(int)> setChoiceMode{this, "setChoiceMode"};
            java::Method<void(bool)> setStackFromBottom{this, "setStackFromBottom"};
        };
    }
}
