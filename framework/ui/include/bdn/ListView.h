#pragma once

#include <bdn/ListViewDataSource.h>
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
        SimpleNotifier<> &onRefresh();

      public:
        static constexpr char coreTypeName[] = "bdn.ListViewCore";

      public:
        ListView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~ListView() override = default;

      public:
        void reloadData();
        void refreshDone();

      public:
        String viewCoreTypeName() const override;

      protected:
        void bindViewCore() override;

      private:
        SimpleNotifier<> _refreshNotifier;
        WeakCallback<void()>::Receiver _refreshCallback;
    };
}
