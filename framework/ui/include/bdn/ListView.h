#pragma once

#include <bdn/ListViewDataSource.h>
#include <bdn/View.h>

namespace bdn
{
    class ListView : public View
    {
      public:
        Property<std::shared_ptr<ListViewDataSource>> dataSource;
        Property<std::optional<size_t>> selectedRowIndex;

      public:
        static constexpr char coreTypeName[] = "bdn.ListViewCore";

      public:
        ListView(std::shared_ptr<UIProvider> uiProvider = nullptr);
        virtual ~ListView() = default;

      public:
        void reloadData();

      public:
        String viewCoreTypeName() const override;

      protected:
        virtual void bindViewCore() override;
    };
}
