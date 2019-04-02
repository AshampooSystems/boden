#include <bdn/ListView.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(ListView)
    }

    ListView::ListView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(ListView, View::viewCoreFactory());
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
        listCore->enableRefresh.bind(enableRefresh, BindMode::unidirectional);

        _refreshCallback = listCore->_refreshCallback.set([=]() { onRefresh().notify(); });
    }

    Notifier<> &ListView::onRefresh() { return _refreshNotifier; }
}
