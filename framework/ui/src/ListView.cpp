#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>

namespace bdn
{

    ListView::ListView(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider)) {}

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
