#pragma once

#include <bdn/android/ViewCore.h>
#include <bdn/android/JListView.h>
#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>

#include <bdn/android/JNativeListAdapter.h>

namespace bdn
{
    namespace android
    {
        class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
        {
          public:
            ListViewCore(std::shared_ptr<ListView> outerView);

            virtual void setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource) override;
            virtual void reloadData() override;

          private:
            static JView createJListView(std::shared_ptr<ListView> outer);

            JListView _jListView;
            JNativeListAdapter _jNativeListAdapter;
        };
    }
}
