#pragma once

#include <bdn/android/wrapper/RecyclerView.h>
#include <bdn/android/wrapper/View.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeListViewClassName[] = "io/boden/android/NativeListView";

    class NativeListView : public BaseView<kNativeListViewClassName>
    {
      public:
        using BaseView<kNativeListViewClassName>::BaseView;

      public:
        JavaMethod<RecyclerView()> getRecyclerView{this, "getRecyclerView"};
        JavaMethod<void(bool)> setEnabled{this, "setEnabled"};
        JavaMethod<void(bool)> setRefreshing{this, "setRefreshing"};
        JavaMethod<void()> reloadData{this, "reloadData"};
        JavaMethod<void(bool)> setEnableSwipeToDelete{this, "setEnableSwipeToDelete"};
        JavaMethod<int(bdn::android::wrapper::View)> getRowIndexForView{this, "getRowIndexForView"};
    };
}