#pragma once

#include <bdn/android/wrapper/ListView.h>
#include <bdn/android/wrapper/View.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeListViewClassName[] = "io/boden/android/NativeListView";

    class NativeListView : public BaseView<kNativeListViewClassName>
    {
      public:
        using BaseView<kNativeListViewClassName>::BaseView;

      public:
        JavaMethod<ListView()> getListView{this, "getListView"};
        JavaMethod<void(bool)> setEnabled{this, "setEnabled"};
        JavaMethod<void(bool)> setRefreshing{this, "setRefreshing"};
    };
}