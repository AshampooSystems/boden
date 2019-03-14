#pragma once

#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/ListView.h>

#include <bdn/android/wrapper/NativeListAdapter.h>

namespace bdn::android
{
    class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
    {
      public:
        ListViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);

        virtual void reloadData() override;

      private:
        wrapper::ListView _jListView;
        wrapper::NativeListAdapter _jNativeListAdapter;
    };
}
