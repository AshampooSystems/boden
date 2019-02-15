#include <bdn/android/JNativeAdapterViewOnItemClickListener.h>
#include <bdn/android/JNativeListAdapter.h>
#include <bdn/android/ListViewCore.h>

namespace bdn
{
    namespace android
    {
        ListViewCore::ListViewCore(std::shared_ptr<ListView> outer)
            : ViewCore(outer, ViewCore::createAndroidViewClass<JListView>(outer)), _jListView(getJViewAS<JListView>()),
              _jNativeListAdapter(getJView())
        {
            _jListView.setDescendantFocusability(0x00060000);
            _jListView.setStackFromBottom(false); // I don't know what I'm doing
            _jListView.setChoiceMode(0x00000001);
            _jListView.setAdapter(JListAdapter(_jNativeListAdapter.getRef_()));

            bdn::android::JNativeAdapterViewOnItemClickListener listener2;
            _jListView.setOnItemClickListener(listener2.cast<OnItemClickListener>());
        }

        void ListViewCore::reloadData() { _jNativeListAdapter.notifyDataSetChanged(); }
    }
}
