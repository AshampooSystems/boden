#include "TodoListData.h"
#include "TodoListItem.h"

#include <bdn/ui.h>

using namespace bdn::ui;

void TodoListDataSource::add(const std::string &entry)
{
    _entries.push_back({entry, false});
    empty = false;
}

void TodoListDataSource::remove(int index)
{
    assert(index >= 0 && index < _entries.size());
    _entries.erase(_entries.begin() + index);

    if (_entries.empty()) {
        empty = true;
    }
}

size_t TodoListDataSource::numberOfRows() { return _entries.size(); }

std::shared_ptr<bdn::ui::View> TodoListDataSource::viewForRowIndex(size_t rowIndex,
                                                                   std::shared_ptr<bdn::ui::View> reusableView)
{
    if (!reusableView) {
        reusableView = std::make_shared<TodoListItem>(bdn::needsInit);
    }

    if (auto item = std::dynamic_pointer_cast<TodoListItem>(reusableView)) {
        item->text = _entries.at(rowIndex).text;
        item->completed.onChange().unsubscribeAll();
        item->completed = _entries.at(rowIndex).completed;
        item->completed.onChange() += [=](const auto &property) {
            _entries.at(rowIndex).completed = property.get();
            _entryCompletedChanged.notify(rowIndex, property.get());
        };

#ifdef BDN_PLATFORM_OSX
        item->_deleteButton->onClick().unsubscribeAll();
        item->_deleteButton->onClick().subscribe([rowIndex, this](const auto &) {
            this->remove(rowIndex);
            _onChange.notify();
        });
#endif
    }

    return reusableView;
}

float TodoListDataSource::heightForRowIndex(size_t rowIndex) { return 40; }
