#pragma once

#include <bdn/ListViewDataSource.h>
#include <bdn/Notifier.h>
#include <bdn/UIUtil.h>
#include <bdn/View.h>

namespace bdn
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

      public:
        Notifier<> &onRefresh();

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
        WeakCallback<void()>::Receiver _refreshCallback;

      public:
        class Core
        {
            friend class ListView;

          public:
            Property<bool> enableRefresh;
            Property<std::shared_ptr<ListViewDataSource>> dataSource;
            Property<std::optional<size_t>> selectedRowIndex;

          public:
            virtual void reloadData() = 0;
            virtual void refreshDone() = 0;

          protected:
            WeakCallback<void()> _refreshCallback;
        };
    };
}
