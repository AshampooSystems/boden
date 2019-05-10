#pragma once

#include <bdn/ui/ListView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/ListView.h>
#include <bdn/android/wrapper/NativeListView.h>

#include <bdn/android/wrapper/NativeListAdapter.h>

namespace bdn::ui::android
{
    class ListViewCore : public ViewCore, virtual public ListView::Core
    {
      public:
        ListViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

        void reloadData() override;
        void refreshDone() override;

        void fireRefresh();

      protected:
        void initTag() override;

      private:
        bdn::android::wrapper::NativeListView _jNativeListView;
        bdn::android::wrapper::ListView _jListView;
        bdn::android::wrapper::NativeListAdapter _jNativeListAdapter;
    };
}
