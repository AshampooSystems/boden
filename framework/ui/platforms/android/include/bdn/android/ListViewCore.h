#pragma once

#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>
#include <bdn/android/JListView.h>
#include <bdn/android/ViewCore.h>

#include <bdn/android/JNativeListAdapter.h>

namespace bdn
{
    namespace android
    {
        class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
        {
          public:
            ListViewCore(std::shared_ptr<ListView> outer);

            virtual void reloadData() override;

          private:
            JListView _jListView;
            JNativeListAdapter _jNativeListAdapter;
        };
    }
}
