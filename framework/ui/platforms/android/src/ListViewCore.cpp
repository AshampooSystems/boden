#include "../include/bdn/android/ListViewCore.h"
#include <bdn/android/ListViewCore.h>
#include <bdn/android/wrapper/NativeAdapterViewOnItemClickListener.h>
#include <bdn/android/wrapper/NativeListAdapter.h>
#include <bdn/android/wrapper/NativeListView.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(ListView, bdn::ui::android::ListViewCore, ListView)
}

namespace bdn::ui::android
{
    ListViewCore::ListViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeListView>(viewCoreFactory)),
          _jNativeListView(getJViewAS<bdn::android::wrapper::NativeListView>()),
          _jListView(_jNativeListView.getListView()),
          _jNativeListAdapter(_jListView.cast<bdn::android::wrapper::View>())
    {
        _jListView.setDescendantFocusability(0x00060000);
        _jListView.setChoiceMode(0x00000001);
        _jListView.setAdapter(bdn::android::wrapper::ListAdapter(_jNativeListAdapter.getRef_()));

        bdn::android::wrapper::NativeAdapterViewOnItemClickListener listener;
        _jListView.setOnItemClickListener(listener.cast<bdn::android::wrapper::OnItemClickListener>());

        enableRefresh.onChange() += [this](auto &property) { _jNativeListView.setEnabled(property.get()); };
    }

    void ListViewCore::refreshDone() { _jNativeListView.setRefreshing(false); }

    void ListViewCore::fireRefresh() { _refreshCallback.fire(); }

    void ListViewCore::initTag()
    {
        ViewCore::initTag();

        auto tag = bdn::java::wrapper::NativeWeakPointer(shared_from_this());
        _jListView.setTag(JavaObject(tag.getRef_()));
    }

    void ListViewCore::reloadData() { _jNativeListAdapter.notifyDataSetChanged(); }
}
