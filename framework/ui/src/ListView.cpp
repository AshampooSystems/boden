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

    String ListView::viewCoreTypeName() const { return coreTypeName; }

    void ListView::bindViewCore()
    {
        View::bindViewCore();
        auto listCore = core<ListViewCore>();
        listCore->_uiProvider = uiProvider();
        listCore->dataSource.bind(dataSource);
        listCore->selectedRowIndex.bind(selectedRowIndex);
    }
}
