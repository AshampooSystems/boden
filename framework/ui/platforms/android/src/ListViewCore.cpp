#include <bdn/android/JNativeAdapterViewOnItemClickListener.h>
#include <bdn/android/JNativeListAdapter.h>
#include <bdn/android/ListViewCore.h>

namespace bdn
{
    namespace android
    {
        ListViewCore::ListViewCore(std::shared_ptr<ListView> outer)
            : ViewCore(outer, createAndroidViewClass<JListView>(outer)), _jListView(getJViewAS<JListView>()),
              _jNativeListAdapter(getJView())
        {
            _jListView.setDescendantFocusability(0x00060000);
            _jListView.setChoiceMode(0x00000001);
            _jListView.setAdapter(JListAdapter(_jNativeListAdapter.getRef_()));

            bdn::android::JNativeAdapterViewOnItemClickListener listener;
            _jListView.setOnItemClickListener(listener.cast<OnItemClickListener>());
        }

        void ListViewCore::reloadData() { _jNativeListAdapter.notifyDataSetChanged(); }
    }
}
