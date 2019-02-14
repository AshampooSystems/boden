#pragma once

#import <bdn/ios/ViewCore.hh>
#include <bdn/ListViewCore.h>
#include <bdn/ListView.h>

@class ListViewDelegateIOS;

namespace bdn
{
    namespace ios
    {
        class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
        {
          public:
            ListViewCore(std::shared_ptr<ListView> outerListView);

            virtual void reloadData() override;

          private:
            static UITableView *createUITableView(std::shared_ptr<ListView> outerListView);

            ListViewDelegateIOS *_nativeDelegate;
        };
    }
}
