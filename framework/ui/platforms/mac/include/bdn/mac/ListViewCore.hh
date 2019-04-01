#pragma once

#include <bdn/ListView.h>
#include <bdn/mac/ViewCore.hh>

@class ListViewDelegateMac;

namespace bdn::mac
{
    class ListViewCore : public ViewCore, virtual public bdn::ListView::Core
    {
      public:
        ListViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~ListViewCore();

      public:
        void init() override;

        void reloadData() override;
        void refreshDone() override;

      public:
        void fireRefresh();

      private:
        static NSScrollView *createNSTableView();

        ListViewDelegateMac *_nativeDelegate;
        NSTableView *_nsTableView;
    };
}
