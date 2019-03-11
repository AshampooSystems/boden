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
        ListViewCore();
        virtual ~ListViewCore();

        virtual void init() override;

        virtual void reloadData() override;

      private:
        static NSScrollView *createNSTableView();

        ListViewDelegateMac *_nativeDelegate;
        NSTableView *_nsTableView;
    };
}
