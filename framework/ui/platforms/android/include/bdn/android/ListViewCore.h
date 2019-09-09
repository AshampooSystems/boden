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

      public:
        std::optional<size_t> rowIndexForView(const std::shared_ptr<View> &view) const override;

      public:
        void reloadData() override;
        void refreshDone() override;

        void fireRefresh();

        void fireDelete(size_t position);

      protected:
        void initTag() override;

      private:
        bdn::android::wrapper::NativeListView _jNativeListView;
        bdn::android::wrapper::RecyclerView _jRecyclerView;
    };
}
