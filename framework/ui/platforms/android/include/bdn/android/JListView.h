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

            Method<void(int)> setDescendantFocusability{this, "setDescendantFocusability"};
            Method<void(int)> setChoiceMode{this, "setChoiceMode"};
            Method<void(bool)> setStackFromBottom{this, "setStackFromBottom"};
        };
    }
}
