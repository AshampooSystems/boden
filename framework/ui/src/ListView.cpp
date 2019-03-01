#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>

namespace bdn
{
    ListView::ListView() {}

    void ListView::reloadData()
    {
        std::shared_ptr<ListViewCore> core = std::dynamic_pointer_cast<ListViewCore>(getViewCore());

        if (core == nullptr) {
            return;
        }

        core->reloadData();
    }

    String ListView::getViewCoreTypeName() const { return coreTypeName; }
}
