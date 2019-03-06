#include <bdn/android/ListViewCore.h>
#include <bdn/android/wrapper/NativeAdapterViewOnItemClickListener.h>
#include <bdn/android/wrapper/NativeListAdapter.h>

namespace bdn::android
{
    ListViewCore::ListViewCore(std::shared_ptr<ListView> outer)
        : ViewCore(outer, createAndroidViewClass<wrapper::ListView>(outer)),
          _jListView(getJViewAS<wrapper::ListView>()), _jNativeListAdapter(getJView())
    {
        _jListView.setDescendantFocusability(0x00060000);
        _jListView.setChoiceMode(0x00000001);
        _jListView.setAdapter(wrapper::ListAdapter(_jNativeListAdapter.getRef_()));

        wrapper::NativeAdapterViewOnItemClickListener listener;
        _jListView.setOnItemClickListener(listener.cast<wrapper::OnItemClickListener>());
    }

    void ListViewCore::reloadData() { _jNativeListAdapter.notifyDataSetChanged(); }
}
