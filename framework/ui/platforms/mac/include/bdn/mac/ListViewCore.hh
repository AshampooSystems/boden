#pragma once

#include <bdn/mac/ChildViewCore.hh>
#include <bdn/ListViewCore.h>
#include <bdn/ListView.h>

@class ListViewDelegateMac;

namespace bdn
{
    namespace mac
    {
        class ListViewCore : public ChildViewCore, virtual public bdn::ListViewCore
        {
          public:
            ListViewCore(std::shared_ptr<ListView> outerListView);

            virtual void reloadData() override;

          private:
            static NSScrollView *createNSTableView(std::shared_ptr<ListView> outerListView);

            ListViewDelegateMac *_nativeDelegate;
            NSTableView *_nsTableView;
        };
    }
}
