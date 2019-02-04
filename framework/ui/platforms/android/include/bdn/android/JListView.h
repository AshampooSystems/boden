#pragma once

#include <bdn/android/JViewGroup.h>
#include <bdn/android/JNativeListAdapter.h>

namespace bdn
{
    namespace android
    {

        constexpr const char kListViewClassName[] = "android/widget/ListView";

        /** Accessor for Java android.widget.ListView objects.*/
        class JListView : public JBaseViewGroup<kListViewClassName>
        {
          public:
            using JBaseViewGroup<kListViewClassName>::JBaseViewGroup;

            Method<void(JListAdapter)> setAdapter{this, "setAdapter"};
        };
    }
}
