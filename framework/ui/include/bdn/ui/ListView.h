#pragma once

#include <bdn/Notifier.h>
#include <bdn/ui/ListViewDataSource.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(ListView)
    }

    class ListView : public View
    {
      public:
        Property<std::shared_ptr<ListViewDataSource>> dataSource;
        Property<std::optional<size_t>> selectedRowIndex;
        Property<bool> enableRefresh;
        Property<bool> enableSwipeToDelete;

      public:
        Notifier<> &onRefresh();
        Notifier<size_t> &onDelete() { return _onDelete; }

      public:
        ListView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~ListView() override = default;

      public:
        void reloadData();
        void refreshDone();

      protected:
        void bindViewCore() override;

      private:
        Notifier<> _refreshNotifier;
        Notifier<size_t> _onDelete;

        WeakCallback<void()>::Receiver _refreshCallback;
        WeakCallback<void(size_t)>::Receiver _deleteCallback;

      public:
        class Core
        {
            friend class ListView;

          public:
            Property<bool> enableRefresh;
            Property<std::shared_ptr<ListViewDataSource>> dataSource;
            Property<std::optional<size_t>> selectedRowIndex;
            Property<bool> enableSwipeToDelete;

          public:
            virtual void reloadData() = 0;
            virtual void refreshDone() = 0;

          protected:
            WeakCallback<void()> _refreshCallback;
            WeakCallback<void(size_t)> _deleteCallback;
        };
    };
}
