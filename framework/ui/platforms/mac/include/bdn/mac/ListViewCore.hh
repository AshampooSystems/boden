#pragma once

#include <bdn/mac/ChildViewCore.hh>
#include <bdn/ListViewCore.h>
#include <bdn/ListView.h>

@class ListViewDataSourceMac;

namespace bdn
{
    namespace mac
    {
        class ListViewCore : public ChildViewCore, virtual public bdn::ListViewCore
        {
          public:
            ListViewCore(std::shared_ptr<ListView> outerListView);

            virtual void setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource) override;
            virtual void reloadData() override;

          private:
            static NSScrollView *createNSTableView(std::shared_ptr<ListView> outerListView);

            ListViewDataSourceMac *_nativeDataSource;
            NSTableView *_nsTableView;
        };
    }
}
