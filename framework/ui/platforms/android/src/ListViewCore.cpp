#include <bdn/android/ListViewCore.h>
#include <bdn/android/JNativeListAdapter.h>

namespace bdn
{
    namespace android
    {
        ListViewCore::ListViewCore(std::shared_ptr<ListView> outer)
            : ViewCore(outer, ViewCore::createAndroidViewClass<JListView>(outer)), _jListView(getJViewAS<JListView>()),
              _jNativeListAdapter(getJView())
        {
            _jListView.setAdapter(JListAdapter(_jNativeListAdapter.getRef_()));
        }

        void ListViewCore::setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource) {}

        void ListViewCore::reloadData() { _jNativeListAdapter.notifyDataSetChanged(); }
    }
}
