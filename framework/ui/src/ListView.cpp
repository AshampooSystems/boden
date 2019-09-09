#include <bdn/ui/ListView.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(ListView)
    }

    ListView::ListView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(ListView, View::viewCoreFactory());
    }

    std::optional<size_t> ListView::rowIndexForView(const std::shared_ptr<View> &view) const
    {
        auto listCore = core<ListView::Core>();
        return listCore->rowIndexForView(view);
    }

    void ListView::reloadData()
    {
        auto listCore = core<ListView::Core>();
        listCore->reloadData();
    }

    void ListView::refreshDone()
    {
        auto listCore = core<ListView::Core>();
        listCore->refreshDone();
    }

    void ListView::bindViewCore()
    {
        View::bindViewCore();
        auto listCore = core<ListView::Core>();
        listCore->dataSource.bind(dataSource);
        listCore->selectedRowIndex.bind(selectedRowIndex);
        listCore->enableRefresh.bind(enableRefresh);
        listCore->enableSwipeToDelete.bind(enableSwipeToDelete);
        listCore->listView = std::static_pointer_cast<ListView>(shared_from_this());

        _refreshCallback = listCore->_refreshCallback.set([this]() { onRefresh().notify(); });
        _deleteCallback = listCore->_deleteCallback.set([this](int index) { onDelete().notify(index); });
    }

    Notifier<> &ListView::onRefresh() { return _refreshNotifier; }
}
