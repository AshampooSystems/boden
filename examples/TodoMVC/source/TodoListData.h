#pragma once

#include <bdn/Notifier.h>
#include <bdn/platform.h>
#include <bdn/ui/ListViewDataSource.h>

class TodoListDataSource : public bdn::ui::ListViewDataSource
{
  public:
    struct Entry
    {
        std::string text;
        bool completed;
    };

    bdn::Property<bool> empty = true;

  public:
    void add(const std::string &entry);
    void remove(int index);

  public:
    size_t numberOfRows() override;
    std::shared_ptr<bdn::ui::View> viewForRowIndex(size_t rowIndex,
                                                   std::shared_ptr<bdn::ui::View> reusableView) override;
    float heightForRowIndex(size_t rowIndex) override;

    bdn::Notifier<size_t, bool> &entryCompletedChanged() { return _entryCompletedChanged; }

#ifdef BDN_PLATFORM_OSX
    bdn::Notifier<> &onChange() { return _onChange; }
#endif

  private:
    std::vector<Entry> _entries;
    bdn::Notifier<size_t, bool> _entryCompletedChanged;

#ifdef BDN_PLATFORM_OSX
    bdn::Notifier<> _onChange;
#endif
};
