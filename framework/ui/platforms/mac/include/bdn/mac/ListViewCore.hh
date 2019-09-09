#pragma once

#include <bdn/mac/ViewCore.hh>
#include <bdn/ui/ListView.h>

@class ListViewDelegateMac;

namespace bdn::ui::mac
{
    class ListViewCore : public ViewCore, virtual public ListView::Core
    {
      public:
        ListViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~ListViewCore();

      public:
        void init() override;

        void reloadData() override;
        void refreshDone() override;

        std::optional<size_t> rowIndexForView(const std::shared_ptr<View> &view) const override;

      public:
        void fireRefresh();

      private:
        static NSScrollView *createNSTableView();

        ListViewDelegateMac *_nativeDelegate;
        NSTableView *_nsTableView;
    };
}
