#include "../include/bdn/android/ListViewCore.h"
#include <bdn/android/ListViewCore.h>
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
          _jRecyclerView(_jNativeListView.getRecyclerView())
    {
        enableRefresh.onChange() += [this](auto &property) { _jNativeListView.setEnabled(property.get()); };
        enableSwipeToDelete.onChange() += [this](auto &property) { _jNativeListView.setEnableSwipeToDelete(true); };
    }

    std::optional<size_t> ListViewCore::rowIndexForView(const std::shared_ptr<View> &view) const
    {
        if (auto viewCore = view->core<ViewCore>()) {
            auto non_const = const_cast<bdn::android::wrapper::NativeListView *>(&_jNativeListView);
            int pos = non_const->getRowIndexForView(viewCore->getJView());

            if (pos != -1) {
                return pos;
            }
        }
        return std::nullopt;
    }

    void ListViewCore::refreshDone() { _jNativeListView.setRefreshing(false); }

    void ListViewCore::fireRefresh() { _refreshCallback.fire(); }

    void ListViewCore::fireDelete(size_t position) { _deleteCallback.fire(position); }

    void ListViewCore::initTag()
    {
        ViewCore::initTag();

        auto tag = bdn::java::wrapper::NativeWeakPointer(shared_from_this());
        _jRecyclerView.setTag(JavaObject(tag.getRef_()));
    }

    void ListViewCore::reloadData() { _jNativeListView.reloadData(); }
}
