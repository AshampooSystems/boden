#pragma once

#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>
#include <bdn/mac/ViewCore.hh>

@class ListViewDelegateMac;

namespace bdn::mac
{
    class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
    {
      public:
        ListViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~ListViewCore() override;

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
