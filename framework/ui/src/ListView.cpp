#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>

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
        auto listCore = core<ListViewCore>();
        listCore->reloadData();
    }

    void ListView::refreshDone()
    {
        auto listCore = core<ListViewCore>();
        listCore->refreshDone();
    }

    String ListView::viewCoreTypeName() const { return String((const char *)coreTypeName); }

    void ListView::bindViewCore()
    {
        View::bindViewCore();
        auto listCore = core<ListViewCore>();
        listCore->dataSource.bind(dataSource);
        listCore->selectedRowIndex.bind(selectedRowIndex);
        listCore->enableRefresh.bind(enableRefresh, BindMode::unidirectional);

        _refreshCallback = listCore->_refreshCallback.set([=]() { onRefresh().notify(); });
    }

    SimpleNotifier<> &ListView::onRefresh() { return _refreshNotifier; }
}
