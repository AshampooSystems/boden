#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>

namespace bdn
{
    ListView::ListView()
    {
        dataSource.onChange() += View::CorePropertyUpdater<std::shared_ptr<ListViewDataSource>, ListViewCore>{
            this, &ListViewCore::setDataSource};
    }

    void ListView::reloadData()
    {
        std::shared_ptr<ListViewCore> core = std::dynamic_pointer_cast<ListViewCore>(getViewCore());

        if (core == nullptr) {
            return;
        }

        core->reloadData();
    }

    String ListView::getListViewCoreTypeName() { return "bdn.ListViewCore"; }

    String ListView::getCoreTypeName() const { return ListView::getListViewCoreTypeName(); }
}
